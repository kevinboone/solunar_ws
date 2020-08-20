# solunar\_ws

`solunar_ws` is a demonstration of a REST-based webservice implemented in C,
and intended to be deployed as a container on OpenShift (or possibly
Kubernetes). It is a "micro-microservice" -- a lightweight container that
provides genuine, albeit highly particular, functionality in a total
container size of about 10Mb. This 10Mb include the operating system layer,
the application binary and dependencies, and (in this particular case) the
complete world timezone database. Even under load, the application does not
use more than a few Mb of RAM in the container.

The webservice provides sun and moon rise and set information in a specific
city on a specified day. It is invoked using a URL of this form:

    http://host:8080/day/[city]/[date]

and produces its results in JSON format. `city` is a city name like "London",
"Minsk", or "Adelaide".
The full list of cities is derived from a Linux timezone database, and is in
the file `libsolunar/src/cityinfo.h`. The date is in the form 'month day
[year]', e.g., 'jun 22 2020'. The spaces, of course, will need to be escaped
in the HTTP URL.

`solunar_ws` uses GNU `libmicrohttpd` as its HTTP engine. 

`solunar_ws` is not a heavyweight business component but, at ~8000 lines of
C, it's a lot more substantial than "Hello, World". It demonstrates how to
use a multi-stage Docker (Podman, etc) build to create a very lightweight
container.

## Building locally

This application builds on various Linux systems. I've mostly tested it on
Fedora 30, but ultimately it is designed to run in an OpenShift container
based on Alpine.  Alpine is a very minimal Linux that uses the MUSL C
library and Busybox.  The use of MUSL in particular can cause some
difficulties, particularly for those of us who have come to rely on
GLIBC-specific extensions when developing for Linux. The use of Alpine for
the container does not prevent development or testing on a more mainstream
Linux, and we can use a multi-stage Docker build to compile against Alpine
in the container itself. However, during development it helps a lot to have
access to a development environment, perhaps in a virtual machine, that uses
the exact same Linux.

`solunar_ws` uses `libmicrohttpd` for its HTTP support, so you'll need this
component, including its development headers. For testing purposes you can
do `dnf install libmicrohttpd-devel` or, on Alpine, `apk add
libmicrohttpd-dev`.However, when building the container image I build
`libmicrohttpd` from  source, to limit its inclusion of unnecessary
dependencies -- more on this later.

On Alpine you'll also need to install the development bits for `zlib`:

    # apk add zlib-dev

Most other Linux varieties seem to have this pre-installed.

With `libmicrohttpd` and `zlib-dev` installed, building locally should 
be as simple as:

    $ make

## Testing locally

    $ ./solunar_ws
    INFO solunar_ws.main: host=0.0.0.0, port=8080
    INFO solunar_ws.main: HTTP server starting

By default, the application listens for HTTP requests on port 8080.
Use Curl, or a web browser, to make a request for

    http://localhost:8080/day/london/jun%2020

The `%20` is the space between "jun" and "2020".

*To test fully on Alpine you'll need to install the timezone database.*
To keep the size down neither the Alpine desktop installer nor the Alpine
Docker image contain the usual timezone database. 

    # apk add tzdata

To check that the timezone data is present and correct, try requesting other
cities. `solunar_ws` is designed to show local times at the specified
location, corrected where necessary for daylight saving. If you're finding
that sunrise is a 10pm, then probably the timezone database is missing. Or
you're in the Arctic.

## Building for the container 

There are many ways to build _for_ the container, and also many ways
to build _in_ the container. Building _for_ the container amounts to
building on a system that has the same Linux and same libraries that
will be used in the container, and then just copying the compiled
binaries, and their dependencies, into the container image.

There's nothing wrong with this approach, but it does mean setting up a
development environment that is likely to be unfamiliar. Building _in_ the
container requires no additional installation, but it time-consuming because
a large amount of development material has to be installed in the container.
It's really an approach that is only practicable if you can do most of the
development and testing out-of-container, and just build in-container for
the final production image.

In the source code bundle is a `Dockerfile` that will build the `solunar_ws`
application and the `libmicrohttpd` library. I generally use Podman, so
building the image amounts to:

    $ podman build .

Note, though, that the in-container-build retrieves the source code for
`solunar_ws` from Github. It would be quicker to populate the 
contained from a local directory, but I prefer to treat the version
on Github (or some other repository) as authoritative. It would otherwise
be too easy to end up with a container image build from source that
is different on different development workstations.

The build consists of two stages. The first stage downloads the source
code for the various components and builds them to binaries. The 
second stage uses exactly the same base layer (Alpine, in this case),
and extracts the compiled binaries from the first stage. Thus we
can build in-container, and still end up with a container that contains
no build tools or superfluous material. 

## Installing on OpenShift 

There are many ways to do this. The way I'm demonstrating here is
to push the generated image to a repository (I'm using
`quay.io`), and create a deployment configuration that retrieves
the image from Quay.

Here are the outline steps to push to Quay and deploy on OpenShift.

    podman build .
    podman image list 

    <none>  <none> 5fc748ec9764  2 minutes ago   10.5 MB
    
    podman tag 5fc748ec9764 solunar_ws:0.1c
    podman login quay.io...
    podman push solunar_ws:0.1c quay.io/kboone/solunar_ws

    oc login...
    oc project...

    oc apply -f solunar_ws.yaml

All being well, this creates a single pod running `solunar_ws`. To access
the webservice outside OpenShift, you'll probably need to create a route.
Note that `solunar_ws` has no TLS logic of its own, so if you want to access
it using an encrypted connection, you'll need to create a secured route with
edge termination. Here's one way to do that:

    $ oc create route edge solunar-ws --service solunar-ws
    $ oc get route
    NAME         HOST/PORT                                     PATH      SERVICES     PORT         TERMINATION   
    solunar-ws   solunar-ws-solunar-ws-test.apps-crc.testing             solunar-ws   solunar-ws   edge         

To test the route:

    curl -k "https://solunar-ws-solunar-ws-test.apps-crc.testing:443/day/london/jun%2022"
    {"city":"Europe/London",
    "timezone city":"Europe/London",
    "latitude":51.5,
    "longitude":-0.116667,
    ....
    "moon phase name":"new",
    "moon phase":0.0272131,
    "moon age":0.80362
    }}

Note that I need to use the `-k` switch to disable the certificate hostname
check, as I haven't installed any certificate that is specific to the 
route, just the default OpenShift certificate. 

Here's the output of `top` run inside the OpenShift pod:

 PID   VSZ    VSZRW RSS (SHR) DIRTY (SHR) STACK COMMAND
    1  1384   508   860   480    92     0   132 /solunar_ws

And, yes, those memory figures are in kB ;)


