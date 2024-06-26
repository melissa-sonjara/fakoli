function installVideoPlayer()
{
	if (typeof videojs == 'function')
	{
		installVideoJS();
	}
	
	if (typeof flowplayerPath != 'undefined' && flowplayerPath) 
	{
		installFlowplayer();
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

		var isMP4 = v.hasClass('MP4');
		
		var play = v.hasClass("autoplay");
		var isLive = v.hasClass("live");

		if (isMP4) return;
		
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
	var resizeVideoJS = function(v)
	{
		if (!v.player) return;
		var videoElt = v.getElement('video');
		if (videoElt == null) return;
		
		var cw = v.getWidth();
		//var h = v.getHeight();
		var auto = v.hasClass('autoplay');
		
		w = v.get('data-width');
		h = v.get('data-height');
		
		var ch = cw / w * h;
		
		v.player.width(cw).height(ch);
		//videoElt.setStyles({'width': cw, 'height': ch});
		v.setStyle('height', ch);
	};
	
	var videos = $$('a.video');
	
	videos.each(function(v)
	{
		var videoElt = v.getElement('video');
		if (videoElt != null) return;
		
		var isMP4 = v.hasClass('MP4');
		if (!isMP4) return;
		
		videoElt = new Element('video');
		videoElt.addClass('video-js').addClass('vjs-default-skin').addClass('vjs-big-play-centered');
		
		var cw = v.getWidth();
		//var h = v.getHeight();
		var auto = v.hasClass('autoplay');
		var responsive = v.hasClass('responsive');
		
		w = v.get('data-width');
		h = v.get('data-height');
		
		var ch;
		
		if (responsive)
		{
			ch = cw / w * h;
		}
		else
		{
			ch = v.getHeight();
		}
		
		//videoElt.setStyles({'width': cw, 'height': ch});
		
		srcElt = new Element('source');
		srcElt.set('src', v.href);
		srcElt.set('type', 'video/mp4');
		v.href='#';
		v.addEvent('click', function() { return true; });

		v.setStyle('height', ch);
		
		videoElt.adopt(srcElt);
		v.adopt(videoElt);
		
		v.player = videojs(videoElt, {controls: true, width: cw, height: ch, autoplay: auto}, function() {});
		var dialog = AbstractDialog.findDialog(v);
		if (dialog)
		{
			dialog.addEvent('resize', function() { resizeVideoJS(v); });
		}
		//resizeVideoJS.delay(10, this, v);
	});
	
	window.addEvent('resize', function()
	{
		var videos = $$('a.video.responsive');
		
		videos.each(function(v)
		{
			resizeVideoJS(v);
		});
	});
	
}

window.addEvent('load', function()
{
	installVideoPlayer();	
});
