var g = new JustGage({
    id: "gauge",
    value: 0,
    decimals: 1,
    label: "[init]",
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
var titlepath = "";
var unit = "";
var conversion = 100000;
var setval = function(newval) {
    var conval = newval / conversion;
    g.refresh(conval);
};
var setconf = function(newskpath, val, min, max ) {
    skpath = newskpath;
    var arr = newskpath.split(".");
    titlepath = "<p>";
    for ( i = 0; i < (arr.length-1); i++ )
        titlepath += arr[i] + ".";
    titlepath += "<b>" + arr[arr.length-1] + "</b>";
    document.getElementById('skPath').innerHTML = titlepath;
    unit = "[bar]"
    var nMin = min || null;
    if (nMin == null)
        nMin = 0;
    var nMax = max || null;
    if (nMax == null)
        nMax = 6
    g.refresh(val, nMax, nMin, unit );
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
