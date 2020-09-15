# Security

This document is completing the public, user guide [Security](../readthedocs/source/security.ipynb#Security). It is defining the implementation of the [Security Policy](../readthedocs/source/security.ipynb#Security-Policy), in more technical terms not necessary relevant to the end user but important for the developer to understand and to agree upon.

## Overlay functions

_DashT_ is a plug-in of [OpenCPN](https://opencpn.org) and is binded to it through an API and an on-init shared library linking mechanism. From the security point of view, it has two functional blocks:

1. The classical,compiled plug-in part which is a part of the OpenCPN application what comes to the security analysis:
  - It is using a binary ABI (application binary interface `opencpn.lib`) defined by an API ;
  - It can create widgets onto the canvas using [wxWidgets](https://www.wxwidgets.org/) library functions ;
  - It can create an graphical overlay onto the canvas using [OpenGL](https://opengl.org/) ;
  - It can read and write routing and waypoint information via the ABI ;
  - It can read, create and modify files in the local file system via wxWidgets library while remaining identified as "OpenCPN".
2. Floating browser technology instruments loosely coupled to "OpenCPN" process
 - OpenCPN provides the window frame;
 - the execution of the JavaScript code via the wxWidgets library belonging equally to "OpenCPN" process ;
 - JavaScript codes communicates with _DashT_ via another, JavaScript text based API proper to _DashT_.

## Data flow

See the [corresponding user's guide section](../readthedocs/source/security.ipynb#Data-flow).

## Security Policy

See the [corresponding user's guide section](../readthedocs/source/security.ipynb#Security-Policy).

## Security Implementation

The interfaces are categorized first by the threat level they present to inject malicious code and to modify the navigation or other data. An implementation plan is defined for all interface threats.

1. **Signal K server node delta channel** - **LOW**
  - _Risk Assessment_: This is a TCP/IP socket with text based protocol (Signal K / JSON). Since it is based on a subscription, its usage is low volume and data is parsed character by character in various level of parsers, first in JSON parser and then in Signal K parser. Both parsers are incorporated in _DashT_ code base and constantly passed through an external security vulnerability parser ;
  - _Implementation_: Several vulnerabilities, such as non-zero ending strings and similar have been fixed after the reports of those from the external advisory ;
  - _Evolution_ : In case of the inactivity of the development, no commits will be made to trigger the inspection and the external advisories will not be able to inspect the existing code against the new, detected vulnerabilities.
2. **OpenCPN Plug-In ABI** - **HIGH**
  - _Risk Assessment_: The API implementation (ABI) is out of configuration control of _DashT_. It has been developed, in organic manner during the entire lifespan of _OpenCPN_ to meet the functional requirements. There is no security policy or security implementation plan available when this is made. However, the code base is entirely available and inspectable: the external advisory reports several issues in the API, thus applicable but which cannot be unfortunately fixed in the provided ABI. Most serious - quite surprisingly - is the [CWE-126 Buffer Over-read](https://cwe.mitre.org/data/definitions/126.html) but there are others ;
  - _Implementation_: The _DashT_ code base addressing and using the _OpenCPN_ ABI is regularly scanned against the very same vulnerabilities and they are all fixed and the scan is constantly maintained in every commit. When applicable, a copy of the data retrieved from _OpenCPN_ is made and passed through the _DashT_ code on which the security fixes have been made ;
  - _Evolution_ : It is likely but not guaranteed that _OpenCPN_ community publish a security policy and starts implementing fixes to the code which creates a security vulnerability report. Meanwhile, the same inactivity risk of the _DashT_ code itself will remain - more there is development, more there will be security fixes. There is no guarantee that this is a constant activity in this regard.
3. **InfluxDB v2** - **LOW**
  - _Risk Assessment_: InfluxData is a company which has huge number of clients, both paying and free around the world. They have a clear, world-wide [security reporting policy](https://www.influxdata.com/how-to-report-security-vulnerabilities/) ;
  - _Implementation_:  _DashT_ does not incorporate any of the third party software but asks the end user to download and installs it. This makes sure that user gets also the latest security updates ;
  - _Evolution_ The risk can be in the evolution of the client side, interfacing software. This is copied as a snapshot from repositories of InfluxData. It may occur that a security issue is detected in those and we would not be necessarily aware of the fix. However, since this code is integrated, it passes also the consant integration security checks and it is likely that the vulnerability be eventually detected. For this, the development must be continuous. 
4. **C++ / JavaScript instrument API** - **VERY LOW**
  - _Risk Assessment_: This is a proprietary, text base API. In both sides text parsing takes into account the applicable security advisory for both programming platforms ;
  - _Implementation_: The parsing code and command implementing code is passed through the external security advisory during continuous integration - the TypeScript / JavaScript code is passed on two, independent platform. In addition, the _node_modules_ development environment is also checked against vulnerable development modules at each commit. It must be noted that none of this code is addressing external, unknown servers ;
  - _Evolution_ : The threats in web technologies are moving ahead quickly. A pause in the development usually causes an avalanche of reports next time the development will start again. This can go over the capacity of the developer to deal with all proposed fixes and they may be ignored. 

As described in the [Data flow diagram](#Data-flow), the code base is divided in two, both functionally and by programming environment:

1. C++ code running in _OpenCPN_ process space - with independent threads running for input / output communications ;
1. TypeScript / JavaScript code running in _OpenCPN_ process space, interfaced with _libwx_gtk3u_webview_3.0_ library.

This allows better implementation when addressing the assessed risks. For example, if there is a need to address third party applications over networks, they do not have to be done in C++ code base which can be potentially be compromised via the ABI - or which can potentially be risk to the _OpenCPN_ via the very same ABI.

The usage of TypeScript over JavaScript is not reducing the risk of the injection type of vulnerabilities _per se_ - the transpiled code is still JavaScript. But stronger typing and the better code quality which follows is a security asset. Typed parameters, used together with a Mozilla project originated _Sanitizer_ to strip potential attack vectors reduces further the risk of hacker being able to penetrate into the system by injection.

The segregation between two programming environments, one compiled and the other transpiled and then interpreted are connected via a proprietary, text based API. This greatly increases the segregation.

>*Risk Assessment*: users who build themselves _DashT_ from this code base for Linux based system may enable their systems with _libwx_gtk**2**u_webview_3.0_ : this GTK2 library is not supported anymore and is thus a security risk. For this reason, _DashT_ official version does not support GTK2 based systems, only GTK3.

**Each commit which is used in the published, official version is signed by the certified author(s)**.
