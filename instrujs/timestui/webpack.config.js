//Webpack requires this to work with directories
const path =  require('path')
const Extract = require('mini-css-extract-plugin')
const Compresseur = require('terser-webpack-plugin')
const HtmlInstaller = require('html-webpack-plugin')

// This is main configuration object that tells Webpackw what to do.
module.exports = {
    //path to entry paint
    entry: {
        main: path.resolve(__dirname, './src/index.ts')
    },
    //path and filename of the final output
    output: {
        path: path.resolve(__dirname, '../../data/instrujs/timestui'),
        filename: 'bundle.js'
    },
    module: {
        rules: [
            {
                test: /\.ts?$/,
                loader: ['awesome-typescript-loader'],
                include: [path.resolve(__dirname, './src'),
                          path.resolve(__dirname, '../src'),
                          path.resolve(__dirname, './influxdb-client/packages/core/src'),
                          path.resolve(__dirname, './influxdb-client/packages/core/src/impl'),
                          path.resolve(__dirname, './influxdb-client/packages/core/src/query'),
                          path.resolve(__dirname, './influxdb-client/packages/core/src/util'),
                         ],
                exclude: [/(node_modules)/]
            },
            {
                test: /\.js$/,
                loader: 'babel-loader',
                options: {
                    "presets": [
                      ["@babel/preset-env", {
                          "useBuiltIns": "entry",
                          "corejs": {"version": 3, "proposals": true},
                          "debug": false,
                          "targets": {
                              "ie": "11",
                              "safari": "6"
                          }
                      }]
                    ],
                    plugins: ['@babel/plugin-transform-runtime'],
                    cacheDirectory: true
                },
                include: [path.resolve(__dirname, './src'),
                          path.resolve(__dirname, '../src')
                         ],
                exclude: [/(node_modules)/],
            },
            {
                test:/\.(sa|sc|c)ss$/,
                include: [path.resolve(__dirname, './sass'),
                          path.resolve(__dirname, '../sass')],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: Extract.loader
                    },
                    {
                        loader: 'css-loader'
                    },
                    {
                        loader: 'sass-loader',
                        options: {
                            implementation: require('sass')
                        }
                    }
                ]
            },
            {
                test: /\.(png|jpe?g|gif|svg)$/,
                include: [path.resolve(__dirname, '../image')],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: 'file-loader',
                        options: {
                            outputPath: 'images'
                        }
                    }
                ]
            },
            {
                test: /iface\.js$/,
                include: [path.resolve(__dirname, '../src')],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: 'exports-loader'
                    },
                ]
            },
        ],
    },
    resolve: {
      extensions: ['.js', '.ts', '.tsx']
    },
    optimization: {
        minimizer: [
            new Compresseur({
                cache: true,
                parallel: true,
                sourceMap: true
            }),
        ],
    },
    devtool: 'source-map',
    stats: {
        children: false,
        maxModules: 0
    },
    performance: {
        hints: false
    },
    plugins: [
        new Extract({
            filename: 'bundle.css'
        }),
        new HtmlInstaller({
            template: './html/index.html',
            filename: 'index.html'
        }),
    ],
    //default mode is production
    mode: 'development'
}
