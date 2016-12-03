var android_pin = 0;

Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');
});

function lightRequest(android_pin, state){
    var req = new XMLHttpRequest();
    
    req.open('POST', 'http://192.168.1.189:8000/' + android_pin + '/' + state, true);
    //req.open('GET', '10.237.212.103:1500/' + android_pin + "/" + state, true);
    req.send();
    req.onload = function(e) {
        if(req.readyState == 4) {
            //200 - HTTP OK
            if(req.status == 200) {
                console.log(req.responseText);
            }
        }
        
    };
}

Pebble.addEventListener('appmessage', function(e) {
    
    if(e.payload['OffKey']){
        android_pin = e.payload['OffKey'];
        lightRequest(android_pin, "off");
        console.log(android_pin + "off");
    } else if(e.payload['OnKey']) {
        android_pin = e.payload['OnKey'];
        lightRequest(android_pin, "on");
        console.log(android_pin + "on")
    }
});