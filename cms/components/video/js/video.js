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
		videoElt.setStyles({'width': v.getWidth(), 'height': v.getHeight()});
		
		srcElt = new Element('source');
		srcElt.set('src', v.href);
		srcElement.set('type', 'video/mp4');
		
		videoElt.adopt(srcElt);
		v.adopt(videoElt);
		
		videojs(videoElt, {}, function() {});
		
	});
}