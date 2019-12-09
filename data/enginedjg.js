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
    labelFontFamily: "Georgia",
    valueFontColor: "blue",
    valueFontFamily: "Helvetica",
    relativeGaugeSize: true
});
var func = function(newval) {
    g.refresh(newval);
};
window.addEventListener('load', 
  function() { 
    func(0);
  }, false);
