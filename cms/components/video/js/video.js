window.addEvent('load', function()
{
	if (!flowplayerPath) return;
	
	var videos = $$('.video');
	
	videos.each(function(v)
	{
		if (!v.id)
		{
			v.id = 'video_' + Math.floor(Math.random()*1E8);
		}
		
		var play = v.hasClass("autoplay");
		flowplayer(v.id, flowplayerPath, {clip: { autoPlay: play, autoBuffering: true}});
	});
		
});
