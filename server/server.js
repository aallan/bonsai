var http = require('http');
var dispatcher = require('httpdispatcher');
var url = require("url");

var vlc = require('vlc')(['-I', 'dummy', '-V', 'dummy', '--verbose', '1',
		          '--no-video-title-show', '--no-disable-screensaver',
                          '--no-snapshot-preview',]);

var channel = 1;
var channels = 12;
var state = 0;
var vol = 0;

var player = vlc.mediaplayer;

var chan = [
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio1_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio2_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio3_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio4fm_mf_p',
	'http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_scotlandfm_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_walesmw_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_lrbris_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_lrdevon_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_lrguern_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_lrldn_mf_p',
	'http://bbcmedia.ic.llnwd.net/stream/bbcmedia_lrwm_mf_p']
	
const PORT = 8080; 

function handleRequest(request, response){
    try {
        console.log(request.url);
        var parts = request.url.split('/');
        if( parts[1] == "vol" ) {
           setVolume( parts[2] );
           response.writeHead(200, {'Content-Type': 'text/plain'});
           response.end( "volume " + parts[2] );
        } else {
           dispatcher.dispatch(request, response);
        }
    } catch(err) {
        console.log(err);
    }
}

function stopStream() {
       player.stop();
}

function playStream(stream) {
    var media = vlc.mediaFromUrl(stream);
    media.parseSync();
    player.media = media;
    player.play();
}

function setVolume(number) {
    if ( player.media ) {
       var setting = parseInt(number, 10)
       var newVol = Math.round( (setting/55000)*100 );
       if ( newVol > 100 ) neVolw = 100;
       if ( newVol < 0 ) newVol = 0;
       player.setVolume( newVol );
       //console.log( "number = " + number + ", newVol = " + newVol );
    }
}

dispatcher.setStatic('resources');

dispatcher.onGet("/channel/clockwise", function(req, res) {
    var pathname = url.parse(req.url).pathname;
    channel = channel + 1
    if ( channel > 12 ) channel = 1;
    console.log( "channel = " + channel );
    stopStream();
    playStream( chan[channel] );

    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end( pathname + " " + channel );
});

dispatcher.onGet("/channel/counterclock", function(req, res) {
    var pathname = url.parse(req.url).pathname;
    channel = channel - 1
    if ( channel < 1 ) channel = 12;
    console.log( "channel = " + channel );
    stopStream();
    playStream( chan[channel] );

    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end( pathname + " " + channel );
});

dispatcher.onGet("/button", function(req, res) {
    var pathname = url.parse(req.url).pathname;
    if (state == 1 ) {
       state = 0;
    } else { 
       state = 1;
    }
    console.log("button = " + state);

    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end( pathname + " " + state);
});

var server = http.createServer(handleRequest);
server.listen(PORT, function(){
    console.log("Server listening on: http://localhost:%s", PORT);
});    
