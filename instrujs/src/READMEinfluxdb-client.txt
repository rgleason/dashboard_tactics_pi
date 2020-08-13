The reason why the influxdb-client TypeScript libary is here
is that it does not have (currently) a polyfill version
which is needed for oldish browser backends of WebView.

The babel/core-js is used to assure the compatibility with
the old WebView's backends. For that, all modules used needs
to be transpiled with the instrument's code. To mutualize
the code, the library source code is put here.

The usage, importing and loading calling babel/core-js is
defined as usual in webpack.config.js first and since these
are TypeScript modules, in tsconfig.json of the transpiled
module. They just refer to TypeScript modules they need
here, typically query modules.

Caveat: one needs to have a node_modules present in this
        folder for the above transpiling in the module's
        sub-directory. Otherwise the core-js modules will
        not be found.
        This is best handled with a symbolic link into
        the module's own node_modules folder before
        transpiling takes place.

GitHub sudden security warning on 'minimist'-module:
        These are caused by the (actual) influxdb-client
        source package. However, the sub-modules (the
        instruments) using the source code do no have
        their own package.json definition files where
        this is fixed. To get rid off the GitHub security
        warning on successive commits, make sure that your
        .gitignore or repository /exclude contains
        package-lock.json and yarn.lock where the
        'minimist' may still remain referenced with wrong
        version number. For the record, this is how you can
        find this, or other GitHub complaints, in instrujs
        working directory:
        $ find . \( -path ./timestui/node_modules -o -path ./enginedjg/node_modules \) -prune -o -type f -exec grep -H 'minimist'  {} \;
