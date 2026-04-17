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

	var videos = document.querySelectorAll('.video');

	videos.forEach(function(v)
	{
		if (v.tagName == "HTML") return;

		if (!v.id)
		{
			v.id = 'video_' + Math.floor(Math.random() * 1E8);
		}

		var isMP4   = v.classList.contains('MP4');
		var play    = v.classList.contains("autoplay");
		var isLive  = v.classList.contains("live");

		if (isMP4) return;

		if (v.href.indexOf("rtmp:") == 0)
		{
			var conn   = dirname(v.href);
			var stream = basename(v.href);

			if (v.name)
			{
				conn   = v.href;
				stream = v.name;
			}

			v.href = "";

			flowplayer(v.id, flowplayerPath,
			{
				clip:    { autoPlay: play, live: isLive, provider: 'rtmp', url: stream, subscribe: isLive },
				plugins: { rtmp: { url: "flowplayer.rtmp-3.2.9.swf", netConnectionUrl: conn, subscribe: isLive } }
			});
		}
		else
		{
			flowplayer(v.id, flowplayerPath, { clip: { autoPlay: play, autoBuffering: true } });
		}
	});
}

function installVideoJS()
{
	var resizeVideoJS = function(v)
	{
		if (!v.player) return;
		var videoElt = v.querySelector('video');
		if (videoElt == null) return;

		var cw = v.offsetWidth;
		var w  = v.getAttribute('data-width');
		var h  = v.getAttribute('data-height');
		var ch = cw / w * h;

		v.player.width(cw).height(ch);
		v.style.height = ch + 'px';
	};

	var videos = document.querySelectorAll('a.video');

	videos.forEach(function(v)
	{
		var videoElt = v.querySelector('video');
		if (videoElt != null) return;

		var isMP4 = v.classList.contains('MP4');
		if (!isMP4) return;

		videoElt = document.createElement('video');
		videoElt.classList.add('video-js', 'vjs-default-skin', 'vjs-big-play-centered');

		var cw         = v.offsetWidth;
		var auto       = v.classList.contains('autoplay');
		var responsive = v.classList.contains('responsive');
		var w          = v.getAttribute('data-width');
		var h          = v.getAttribute('data-height');
		var ch;

		if (responsive)
		{
			ch = cw / w * h;
		}
		else
		{
			ch = v.offsetHeight;
		}

		var srcElt  = document.createElement('source');
		srcElt.src  = v.href;
		srcElt.type = 'video/mp4';
		v.href = '#';
		v.addEventListener('click', function() { return true; });

		v.style.height = ch + 'px';

		videoElt.appendChild(srcElt);
		v.appendChild(videoElt);

		v.player = videojs(videoElt, { controls: true, width: cw, height: ch, autoplay: auto }, function() {});

		var dialog = AbstractDialog.findDialog(v);
		if (dialog)
		{
			dialog.addEventListener('resize', function() { resizeVideoJS(v); });
		}
	});

	window.addEventListener('resize', function()
	{
		var responsiveVideos = document.querySelectorAll('a.video.responsive');

		responsiveVideos.forEach(function(v)
		{
			resizeVideoJS(v);
		});
	});
}

window.addEventListener('load', function()
{
	installVideoPlayer();
});
