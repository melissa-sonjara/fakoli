window.addEvent('load', function()
{
	installVideoPlayer();	
});

function installVideoPlayer()
{
	if (typeof flowplayerPath != 'undefined' && flowplayerPath) 
	{
		installFlowplayer();
		return;
	}
	
	if (typeof videojs == 'function')
	{
		installVideoJS();
		return;
	}
}

function installFlowplayer()
{
	if (typeof flowplayerPath == 'undefined' || !flowplayerPath) return;
	
	var videos = $$('.video');
	
	videos.each(function(v)
	{
		if (v.tagName == "HTML") return;
		
		if (!v.id)
		{
			v.id = 'video_' + Math.floor(Math.random()*1E8);
		}

		var play = v.hasClass("autoplay");
		var isLive = v.hasClass("live");
		
		if (v.href.indexOf("rtmp:") == 0)
		{
			var conn = dirname(v.href);
			var stream = basename(v.href);
			
			if (v.name)
			{
				conn = v.href;
				stream = v.name;
			}
			
			v.href = "";
			
			flowplayer(v.id, flowplayerPath, 
			{
				clip: {autoPlay: play, live: isLive, provider: 'rtmp', url: stream, subscribe: isLive}, 
				plugins: 
				{ 
					rtmp:  { url: "flowplayer.rtmp-3.2.9.swf", netConnectionUrl: conn, subscribe: isLive}
				}
	        });
		}
		else
		{
			flowplayer(v.id, flowplayerPath, {clip: { autoPlay: play, autoBuffering: true}});
		}
	});
}

function installVideoJS()
{
	var videos = $$('a.video');
	
	videos.each(function(v)
	{
		var videoElt = v.getElement('video');
		if (videoElt != null) return;
		
		videoElt = new Element('video');
		videoElt.addClass('video-js').addClass('vjs-default-skin');
		
		var w = v.getWidth();
		var h = v.getHeight();
		var auto = v.hasClass('autoplay');
		
		if (!w) w = v.get('data-width');
		if (!h) h = v.get('data-height');
		
		videoElt.setStyles({'width': w, 'height': h});
		
		srcElt = new Element('source');
		srcElt.set('src', v.href);
		srcElt.set('type', 'video/mp4');
		v.href='#';
		v.addEvent('click', function() { return true; });
		
		videoElt.adopt(srcElt);
		v.adopt(videoElt);
		
		videojs(videoElt, {controls: true, width: w, height: h, autoplay: auto}, function() {});
		
	});
}