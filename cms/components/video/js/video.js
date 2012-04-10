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
		
		if (v.href.indexOf("rtmp:") == 0)
		{
			flowplayer(v.id, flowplayerPath, 
			{
				clip: {autoPlay: play, provider: 'percy'}, 
				plugins: 
				{ 
					'percy':  { url: "flowplayer.rtmp-3.2.9.swf", netConnectionUrl: v.href}
				}
	        });
		}
		else
		{
			flowplayer(v.id, flowplayerPath, {clip: { autoPlay: play, autoBuffering: true}});
		}
	});
}