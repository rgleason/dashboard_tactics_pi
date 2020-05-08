* $Id: ReadmeUbuntu18Build.txt, v1.0 2019/11/30 DarthVader Exp $

Note: his is an ongoing problem the solution of which is not
     automated for Continuous Integration (with .travis):
     - do not use travis build for "bionic"  18.04.4LTS
       for distribution until this message has disappeared.
     - until then, a manual build can be done using the
       below instruction.

Problem:
========

Ubuntu 18.04LTS and its derivates have a complicated issue with
wxWidgets altenative insallations where:
- either the default is gtk3 based libraries, while OpenCPN v5
  requires gtk2 based wxWidgets ;
- or, if gtk2 is default, the CMake does not find it.

Solution:
========

Check that gtk2-unicode-3.0 is selected

~$ sudo wx-config --selected-config
gtk2-unicode-3.0

If not, change and test again.

~$ sudo  update-alternatives --config wx-config

The PluginConfigure.cmake needs to be modified as follows:

-  SET(wxWidgets_USE_LIBS base core net xml html adv aui)
+  SET(wxWidgets_FIND_COMPONENTS base core net xml html adv aui)

-    FIND_PACKAGE(wxWidgets REQUIRED)
+    FIND_PACKAGE(wxWidgets COMPONENTS ${wxWidgets_FIND_COMPONENTS}

The above appears to work also in Raspian beaver (Debian 10)

Automation in Continuous Integration:
=====================================

Not yet done, as mentioned, but the solution would be something like
in this OpenCPN ci-script for Debian: https://git.io/JfcNi

if [[ "$EXTRA_BUILD_OPTS" == *OCPN_FORCE_GTK3=ON* ]]; then
    sudo update-alternatives --set wx-config \
        /usr/lib/*-linux-*/wx/config/gtk3-unicode-3.0
fi

in this case, change above for gtk2.

To be done but as for now, not urgent.

