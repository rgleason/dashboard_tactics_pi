var g = new JustGage({
    id: "gauge",
    value: 0,
    decimals: 1,
    label: "Right click\nto select",
    min: 0,
    max: 100,
    pointer: true,
    pointerOptions: {
        toplength: -15,
        bottomlength: 10,
        bottomwidth: 12,
        color: '#8e8e93',
        stroke: '#ffffff',
        stroke_width: 3,
        stroke_linecap: 'round'
    },
    labelFontColor: "black",
    valueFontColor: "blue",
    valueFontFamily: "Courier",
    relativeGaugeSize: true
});
var skpath = "";
var setval = function(newval) {
    g.refresh(newval);
};
var setconf = function(newskpath, val, min, max ) {
    skpath = newskpath;
    var arr = newskpath.split(".");
    if ( arr.length > 2)
        var str2 = arr[0] + '.' + arr[1] + '\n';
    for ( i = 2; i < arr.length; i++ )
        str2 += '.' + arr[i]; 
    g.refresh(val, max, min, str2 );
};
var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden'; // webkit
    document.body.scroll = "no"; // ie
}
window.addEventListener('load', 
                        function() {
                            unloadScrollBars();                     
                            setval(0);
                        }, false);
