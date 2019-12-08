var func = function(newval) {
    dataDiv = document.getElementById('currentData');
    dataDiv.innerHTML = newval;
};
window.addEventListener('load', 
  function() { 
    func("<p>- - -</p>");
  }, false);
