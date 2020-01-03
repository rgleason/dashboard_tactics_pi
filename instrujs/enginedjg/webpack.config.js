//Webpack requires this to work with directories
const path =  require('path');
const extract = require("mini-css-extract-plugin");
const compresseur = require('terser-webpack-plugin');
const htmlinstaller = require('html-webpack-plugin');

// This is main configuration object that tells Webpackw what to do. 
module.exports = {
    //path to entry paint
    entry: './src/index.js',
    //path and filename of the final output
    output: {
        path: path.resolve(__dirname, '../../data/instrujs/enginedjg'),
        filename: 'bundle.js'
    },
    module: { 
        rules: [
            {
                test: /\.js$/,
                include: [path.resolve(__dirname, "./src"),
                          path.resolve(__dirname, "../src")],
                exclude: /(node_modules)/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        presets: ['@babel/preset-env'],
                        plugins: ['@babel/plugin-transform-runtime']
                    }
                }
            },
            {
                test:/\.(sa|sc|c)ss$/,
                include: [path.resolve(__dirname, "./sass"),
                          path.resolve(__dirname, "../sass")],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: extract.loader
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
                include: [path.resolve(__dirname, "../image")],
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
                include: [path.resolve(__dirname, "./src")],
                include: [path.resolve(__dirname, "../src")],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: 'exports-loader'
                    },
                ]
            },
        ],
    },
    optimization: {
        minimizer: [
            new compresseur({
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
        new extract({
            filename: 'bundle.css'
        }),
        new htmlinstaller({
            template: './html/index.html',
            filename: 'index.html'
        })
    ],
    //default mode is production
    mode: 'development'
}
