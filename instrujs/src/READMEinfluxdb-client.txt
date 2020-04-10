The reason why the influxdb-client TypeScript libary is here
is that it does not have (currently) a polyfilled version
which is needed for oldish browser backends of WebView.

The babel/core-js is used for that and to mutualize the code,
the library source code is put here.

The usage, importing and loading calling babel/core-js is
defined as usual in webpack.conig.js first and since these
are TypeScript modules, in tsconfig.json of the transpiled
module. They just refer to TypeScript modules they need
here, typically query modules.

Caveat: one needs to have a node_modules present in this
        folder for the above transpiling in the module's
        sub-directory. This is best handled with a symbolic
        link into the module's own node_modules folder.        