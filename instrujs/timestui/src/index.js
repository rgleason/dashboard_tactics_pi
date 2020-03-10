// Create the transitional events (the IE way, sorry!) for clieant messages
var bottom = document.getElementById('bottom');
if (!bottom) {
    throw 'timestui: init: no element: bottom';
}
// All available paths have been set
var eventsetall = document.createEvent('Event');
eventsetall.initEvent('setall', false, false);
//bottom.addEventListener('setall', ((event: Event)=>{return (event: Event)=>{
bottom.addEventListener('setall', (function (event) {
    console.error('Event:  setall: error: timestui does not require all paths');
}));
window.iface.regeventsetid(bottom, eventsetall);
