window.addEvent('load', function()
{
	installFlowplayer();	
});

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
			
			v.href = "";
			
			flowplayer(v.id, flowplayerPath, 
			{
				clip: {autoPlay: play, live: isLive, provider: 'rtmp', url: stream}, 
				plugins: 
				{ 
					rtmp:  { url: "flowplayer.rtmp-3.2.9.swf", netConnectionUrl: conn}
				},
				debug: true,
				log: { level: 'info' }
	        });
		}
		else
		{
			flowplayer(v.id, flowplayerPath, {clip: { autoPlay: play, autoBuffering: true}});
		}
	});
}