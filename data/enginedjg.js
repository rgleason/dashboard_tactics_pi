var g = new JustGage({
    id: "gauge",
    value: 67,
    decimals: 1,
    label: "Oil pressure [bar]",
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
var setval = function(newval) {
    g.refresh(newval);
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
