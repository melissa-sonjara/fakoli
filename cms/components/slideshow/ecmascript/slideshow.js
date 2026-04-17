var Slideshow = (function()
{
	class SlideshowSingleton
	{
		constructor(options)
		{
			// Default options
			this.options = Object.assign(
				{preload: true, showThumbnails: true, showInfo: true, autoPlay: true, transition: 'switch'},
				options
			);

			this.busy = false;
			this.timer = null;
			this.playing = false;
			this.position = 0;
			this.showing = -1;

			this.loadedThumbnails = [];
			this.loadedImages = [];

			this.slideshow = null;
			this.viewport = null;
			this.gallery = null;
			this.progress = null;
			this.bar = null;
			this.progressText = null;
			this.progressBorder = null;
			this.progressCount = null;
			this.strip = null;
			this.scroller = null;
			this.caption = null;
			this.credit = null;
			this.creditText = null;
			this.highlight = null;
			this.playButton = null;
			this.infoButton = null;
			this.spinner = null;

			this.scrollLeftButton = null;
			this.scrollRightButton = null;

			this.zoomOverlay = null;
			this.zoomIn = null;
			this.zoomOut = null;
			this.zoomSlider = null;

			this.zoomMax = 2;
			this.zoomMin = 0.0;
			this.zoomLevel = 0.0;

			this.top = 0;
			this.left = 0;

			this.highlightFX = null;

			this.thumbnails = [];
			this.images = [];
			this.captions = [];
			this.credits = [];

			this.slideshow = document.getElementById('slideshow');
			this.viewport = document.getElementById('viewport');
			this.gallery = document.getElementById('gallery');
			this.progress = document.getElementById('progress');
			this.bar = document.querySelector('#progress .bar');
			this.progressText = document.querySelector('#progress .text');
			this.progressBorder = document.querySelector('#progress .border');
			this.progressCount = document.getElementById('progress_count');
			this.strip = document.getElementById('thumbnail_strip');
			this.scroller = document.getElementById('thumbnail_scroll');
			this.caption = document.getElementById('caption');
			this.credit = document.getElementById('credit');
			this.creditText = document.getElementById('creditText');
			this.highlight = document.getElementById('thumbnail_highlight');
			this.playButton = document.getElementById('play_button');
			this.infoButton = document.getElementById('info_button');

			this.scrollLeftButton = document.getElementById('scroll_left');
			this.scrollRightButton = document.getElementById('scroll_right');

			this.zoomOverlay = document.getElementById('zoom_overlay');
			this.zoomIn = document.getElementById('zoom_in');
			this.zoomOut = document.getElementById('zoom_out');

			this.scrollLeftButton.addEventListener('mouseover', function() { this.scrollLeftButton.style['background-color'] = '#e8ac1c'; }.bind(this));
			this.scrollLeftButton.addEventListener('mouseout', function() { this.scrollLeftButton.style['background-color'] = '#ccc'; }.bind(this));
			this.scrollLeftButton.addEventListener('click', function() { this.scrollThumbnails(-1); }.bind(this));

			this.scrollRightButton.addEventListener('mouseover', function() { this.scrollRightButton.style['background-color'] = '#e8ac1c'; }.bind(this));
			this.scrollRightButton.addEventListener('mouseout', function() { this.scrollRightButton.style['background-color'] = '#ccc'; }.bind(this));
			this.scrollRightButton.addEventListener('click', function() { this.scrollThumbnails(1); }.bind(this));

			// highlightFX: animated movement of the highlight strip — no direct native equivalent;
			// kept as a plain object that exposes a start() shim using CSS transitions
			this.highlightFX = document.getElementById('thumbnail_highlight');

			this.gallery.addEventListener('mouseover', function() { if (!this.playing) this.zoomOverlay.style['opacity'] = 1; }.bind(this));
			this.gallery.addEventListener('mouseout', function() { this.zoomOverlay.style['opacity'] = 0; }.bind(this));

			// Unified wheel event (replaces MooTools wheelup / wheeldown / mousewheel)
			this.gallery.addEventListener('wheel', function(e)
			{
				e.preventDefault();
				if (!this.playing) this.alterZoomLevel((e.deltaY > 0) ? 10 : -10);
			}.bind(this));

			this.playButton.addEventListener('click', function() { this.togglePlay(); }.bind(this));
			this.infoButton.addEventListener('click', function() { this.toggleInfo(); }.bind(this));

			this.scroller.style['position'] = 'absolute';

			// Zoom slider — Slider was a MooTools widget. Wire up a native <input type="range"> if present.
			var zoomScaleEl = document.getElementById('zoom_scale');
			if (zoomScaleEl && zoomScaleEl.tagName.toLowerCase() === 'input')
			{
				this.zoomSlider = zoomScaleEl;
				this.zoomSlider.addEventListener('input', function()
				{
					this.setZoomPercent(parseInt(this.zoomSlider.value, 10));
				}.bind(this));
				this.zoomSlider.value = 100;
			}
			else
			{
				// Fallback stub so callers of this.zoomSlider.set() don't throw
				this.zoomSlider = {
					set: function(v) {}
				};
			}

			this.zoomIn.addEventListener('click', function() { this.alterZoomLevel(-10); }.bind(this));
			this.zoomOut.addEventListener('click', function() { this.alterZoomLevel(10); }.bind(this));

			var galleryRect = this.gallery.getBoundingClientRect();
			var progressRect = this.progress.getBoundingClientRect();
			var top = galleryRect.height / 2 - progressRect.height / 2 + galleryRect.top;
			var left = galleryRect.width / 2 - progressRect.width / 2 + galleryRect.left;

			if (this.options.preload)
			{
				Object.assign(this.progress.style, {
					visibility: 'visible',
					left: left + 'px',
					top: top + 'px',
					opacity: '0.75'
				});
			}

			// Spinner image
			var spinnerImg = new Image();
			spinnerImg.onload = function()
			{
				this.spinner = spinnerImg;
				this.gallery.appendChild(this.spinner);
				this.spinner.style['display'] = 'none';
			}.bind(this);
			spinnerImg.src = "/fakoli/slideshow/images/loading_animation.gif";
		}

		loadThumbnails()
		{
			this.progressText.innerHTML = "Loading thumbnails...";
			var slideshow = this;
			var loaded = 0;
			var total = slideshow.thumbnails.length;

			if (total === 0)
			{
				if (slideshow.options.preload)
				{
					slideshow.loadImages();
				}
				else
				{
					slideshow.selectImage(0);
				}
				return;
			}

			slideshow.thumbnails.forEach(function(src, index)
			{
				var img = new Image();
				img.onload = function()
				{
					var ratio = (img.width > img.height) ? 60 / img.width : 60 / img.height;

					Object.assign(img.style, {
						position: 'relative',
						opacity: '0.75',
						width: (img.width * ratio) + 'px',
						height: (img.height * ratio) + 'px'
					});

					slideshow.loadedThumbnails[slideshow.getIndex(img.src, slideshow.thumbnails)] = img;
					loaded++;
					var progressWidth = parseInt(window.getComputedStyle(slideshow.progress).getPropertyValue('width'), 10);
					var percent = (loaded * progressWidth) / total;
					slideshow.bar.style['width'] = percent + 'px';
					slideshow.progressCount.innerHTML = loaded + " of " + total;

					if (loaded === total)
					{
						if (slideshow.options.preload)
						{
							slideshow.loadImages();
						}
						else
						{
							slideshow.selectImage(0);
						}
					}
				};
				img.src = src;
			});
		}

		loadImages()
		{
			this.progressText.innerHTML = "Loading images...";
			var slideshow = this;
			var loaded = 0;
			var total = slideshow.images.length;

			if (total === 0)
			{
				slideshow.progress.style['visibility'] = 'hidden';
				slideshow.busy = false;
				return;
			}

			slideshow.images.forEach(function(src, index)
			{
				var img = new Image();
				img.onload = function()
				{
					Object.assign(img.style, {
						display: 'block',
						position: 'absolute',
						opacity: '0',
						zIndex: '1',
						visibility: 'visible'
					});

					slideshow.gallery.appendChild(img);
					slideshow.positionImage(img);

					// Drag.Move was a MooTools widget; native drag support via mousedown/mousemove
					img.addEventListener('mousedown', function(e) { e.preventDefault(); });

					slideshow.loadedImages[slideshow.getIndex(img.src, slideshow.images)] = img;
					loaded++;
					var progressWidth = parseInt(window.getComputedStyle(slideshow.progress).getPropertyValue('width'), 10);
					var percent = (loaded * progressWidth) / slideshow.thumbnails.length;
					slideshow.bar.style['width'] = percent + 'px';
					slideshow.progressCount.innerHTML = loaded + " of " + slideshow.thumbnails.length;

					if (loaded === total)
					{
						slideshow.progress.style['visibility'] = 'hidden';

						slideshow.scroller.style['width'] = (slideshow.thumbnails.length * 100) + 'px';
						slideshow.scroller.style['left'] = '0px';
						slideshow.highlight.style['left'] = '-100px';

						slideshow.loadedThumbnails.forEach(function(image, i)
						{
							slideshow.scroller.appendChild(image);
							Object.assign(image.style, {
								left: (i * 20) + 'px',
								top: '10px',
								verticalAlign: 'middle',
								cursor: 'pointer'
							});
							image.addEventListener('mouseover', function() { image.style['opacity'] = '1'; });
							image.addEventListener('mouseout', function() { image.style['opacity'] = '0.75'; });
							image.addEventListener('click', function() { slideshow.clickImage(i); });
						});

						slideshow.busy = false;
						if (slideshow.thumbnails.length > 0) slideshow.selectImage(0);
						if (slideshow.options.autoPlay)
						{
							slideshow.togglePlay();
						}
					}
				};
				img.src = src;
			});
		}

		positionImage(img)
		{
			var galleryRect = this.gallery.getBoundingClientRect();
			var gw = galleryRect.width - 20;
			var gh = galleryRect.height - 20;

			if (img.naturalWidth == undefined) img.naturalWidth = img.width;
			if (img.naturalHeight == undefined) img.naturalHeight = img.height;

			var iw = img.naturalWidth || img.width;
			var ih = img.naturalHeight || img.height;

			var ratioW = gw / iw;
			var ratioH = gh / ih;

			this.ratio = (ratioW < ratioH) ? ratioW : ratioH;

			var viewportRect = this.viewport.getBoundingClientRect();
			var w = viewportRect.width - 20;
			var h = viewportRect.height - 20;

			var l = (w / 2) - (iw * this.ratio / 2) + 10;
			var t = (h / 2) - (ih * this.ratio / 2) + 10;

			Object.assign(img.style, {
				left: l + 'px',
				top: t + 'px',
				width: (iw * this.ratio) + 'px',
				height: (ih * this.ratio) + 'px'
			});

			return this.ratio;
		}

		getZoomPercent()
		{
			var percent = 100 - (100 * (this.zoomLevel - this.zoomMin)) / (this.zoomMax - this.zoomMin);
			return percent;
		}

		alterZoomLevel(amount)
		{
			var percent = this.getZoomPercent() + amount;
			if (percent < 0) percent = 0;
			if (percent > 100) percent = 100;
			this.zoomSlider.set(percent);
		}

		setZoomPercent(percent)
		{
			if (this.showing == -1) return;

			var img = this.loadedImages[this.showing];

			if (!img) return;

			var imgRect = img.getBoundingClientRect();
			var viewportRect = this.viewport.getBoundingClientRect();
			var x = imgRect.left - viewportRect.left;
			var y = imgRect.top - viewportRect.top;

			//console.debug("(%f, %f) %f x %f [%f x %f]", x, y, img.width, img.height, viewportRect.width, viewportRect.height);

			var zx = (viewportRect.width / 2 - x);
			zx = zx / this.zoomLevel;

			var zy = (viewportRect.height / 2 - y);
			zy = zy / this.zoomLevel;

			//console.debug("(%f, %f) : (%f, %f) %f", zoomX, zoomY, zx, zy, zoomLevel);

			this.zoomLevel = (this.zoomMax - this.zoomMin) * (100 - percent) / 100 + this.zoomMin;

			var iw = img.naturalWidth || img.width;
			var ih = img.naturalHeight || img.height;

			var w = iw * this.zoomLevel;
			var h = ih * this.zoomLevel;

			var offX = -(zx * this.zoomLevel) + viewportRect.width / 2;
			var offY = -(zy * this.zoomLevel) + viewportRect.height / 2;

			this.zoomX = zx;
			this.zoomY = zy;

			Object.assign(img.style, {
				top: offY + 'px',
				left: offX + 'px',
				width: w + 'px',
				height: h + 'px'
			});
		}

		transition(i)
		{
			switch (this.options.transition)
			{
			case "fade":

				if (this.showing != -1)
				{
					// Fade out current image using CSS transition
					var outImg = this.loadedImages[this.showing];
					outImg.style.transition = 'opacity 1.5s linear';
					outImg.style.opacity = '0';
				}

				// Fade in new image
				var inImg = this.loadedImages[i];
				inImg.style.transition = 'opacity 1.5s linear';
				inImg.style.opacity = '1';
				break;

			case "switch":
			default:

				if (this.showing != -1)
				{
					this.loadedImages[this.showing].style['opacity'] = '0';
				}
				this.loadedImages[i].style['opacity'] = '1';
				break;
			}
		}

		selectImage(i)
		{
			if (this.busy) return;
			//alert("selectImage(" + i + ")");

			this.transition(i);

			this.caption.innerHTML = this.captions[i];
			this.creditText.innerHTML = this.credits[i];

			this.showing = i;

			this.zoomMin = this.positionImage(this.loadedImages[i]);
			this.zoomLevel = this.zoomMin;
			this.zoomX = this.loadedImages[i].naturalWidth / 2;
			this.zoomY = this.loadedImages[i].naturalHeight / 2;
			this.zoomSlider.set(100);

			this.setZoomPercent(100);
			this.highlight.style['visibility'] = 'visible';
			var l = this.loadedThumbnails[i].offsetLeft - 10;
			var w = this.loadedThumbnails[i].width + 20;

			// Animate highlight position using CSS transition
			this.highlight.style.transition = 'left 0.5s, width 0.5s';
			this.highlight.style.left = l + 'px';
			this.highlight.style.width = w + 'px';

			var remaining = this.strip.offsetWidth - (l + this.scroller.offsetLeft);
			var offset = this.loadedThumbnails[this.showing].offsetLeft - 4;
			if (remaining < 100 || offset < -this.scroller.offsetLeft)
			{
				// Animate scroller using CSS transition
				this.scroller.style.transition = 'left 1s';
				this.scroller.style.left = -offset + 'px';
			}
		}

		selectImageImmediate(i)
		{
			if (this.busy) return;
			if (this.showing != -1)
			{
				this.loadedImages[this.showing].style['opacity'] = '0';
			}

			this.loadedImages[i].style['opacity'] = '1';

			this.caption.innerHTML = this.captions[i];
			this.creditText.innerHTML = this.credits[i];

			this.showing = i;

			this.zoomMin = this.positionImage(this.loadedImages[i]);
			this.zoomLevel = this.zoomMin;
			this.zoomX = this.loadedImages[i].naturalWidth / 2;
			this.zoomY = this.loadedImages[i].naturalHeight / 2;
			this.zoomSlider.set(100);

			this.setZoomPercent(100);

			if (this.options.preload)
			{
				this.highlight.style['visibility'] = 'visible';
				var l = this.loadedThumbnails[i].offsetLeft - 10;
				var w = this.loadedThumbnails[i].width + 20;

				// Animate highlight position using CSS transition
				this.highlight.style.transition = 'left 0.5s, width 0.5s';
				this.highlight.style.left = l + 'px';
				this.highlight.style.width = w + 'px';

				var remaining = this.strip.offsetWidth - (l + this.scroller.offsetLeft);
				var offset = this.loadedThumbnails[this.showing].offsetLeft - 4;
				if (remaining < 100 || offset < -this.scroller.offsetLeft)
				{
					// Animate scroller using CSS transition
					this.scroller.style.transition = 'left 1s';
					this.scroller.style.left = -offset + 'px';
				}
			}
		}

		clickImage(i)
		{
			if (!this.playing && !this.busy)
			{
				this.selectImage(i);
			}
		}

		getIndex(txt, arr)
		{
			for (var i = 0; i < arr.length; ++i)
			{
				if (arr[i] == txt)
				{
					return i;
				}
			}

			return -1;
		}

		scrollThumbnails(dir)
		{
			if (this.busy) return;
			this.position += dir;
			if (this.position <= 0) this.position = 0;
			else if (this.position > this.thumbnails.length - 4) this.position = this.thumbnails.length - 4;

			var offset = this.loadedThumbnails[this.position].offsetLeft;
			// Animate scroller using CSS transition
			this.scroller.style.transition = 'left 1s';
			this.scroller.style.left = -offset + 'px';
		}

		togglePlay()
		{
			if (this.playing)
			{
				this.playing = false;
				this.playButton.style['background-image'] = 'url(/fakoli/slideshow/images/play.gif)';
			}
			else
			{
				this.playing = true;
				this.playButton.style['background-image'] = 'url(/fakoli/slideshow/images/stop.gif)';
				this.playNext();
			}
		}

		toggleInfo()
		{
			var shown = window.getComputedStyle(this.caption).getPropertyValue('display');
			if (shown == "inline")
			{
				this.caption.style['display'] = 'none';
				this.creditText.style['display'] = 'none';
				this.infoButton.style['background-image'] = 'url(/fakoli/slideshow/images/info_off.gif)';
			}
			else
			{
				this.caption.style['display'] = 'inline';
				this.creditText.style['display'] = 'inline';
				this.infoButton.style['background-image'] = 'url(/fakoli/slideshow/images/info.gif)';
			}
		}

		playNext()
		{
			if (!this.playing) return;

			if (this.showing < this.thumbnails.length - 1)
			{
				this.selectImage(this.showing + 1);
			}
			else
			{
				this.selectImage(0);
			}

			this.timer = setTimeout(this.playNext.bind(this), 12000);
		}

		selectImageFromURL(url)
		{
			var idx = this.getIndex(url, this.images);
			this.selectImage(idx);
		}

		clear()
		{
			this.loadedImages.forEach(function(i) { if (i != null) i.style['opacity'] = '0'; });
		}

		showSpinner()
		{
			this.clear();

			this.spinner.style['opacity'] = '1';

			var w = this.spinner.width;
			var h = this.spinner.height;
			var g = this.gallery.getBoundingClientRect();
			var t = (g.height - h) / 2;
			var l = (g.width - w) / 2;
			Object.assign(this.spinner.style, {
				position: 'absolute',
				zIndex: '1',
				top: t + 'px',
				left: l + 'px',
				display: 'block'
			});
		}

		hideSpinner()
		{
			this.spinner.style['display'] = 'none';
		}

		showImage(url)
		{
			this.clear();

			var idx = this.getIndex(url, this.images);
			//this.zoomSlider.refreshLayout();

			if (!this.options.preload)
			{
				if (!this.loadedImages[idx])
				{
					this.showSpinner();

					var slideshow = this;

					var img = new Image();
					img.onload = function()
					{
						Object.assign(img.style, {
							position: 'absolute',
							opacity: '0',
							zIndex: '1'
						});

						slideshow.positionImage(img);
						slideshow.gallery.appendChild(img);

						// Drag.Move was a MooTools widget; native drag support via mousedown/mousemove
						img.addEventListener('mousedown', function(e) { e.preventDefault(); });

						slideshow.loadedImages[idx] = img;
						slideshow.selectImageImmediate(idx);
						slideshow.hideSpinner();
					};
					img.src = url;
				}
				else
				{
					this.selectImageImmediate(idx);
					this.hideSpinner();
				}
			}
			else
			{
				this.selectImageImmediate(idx);
				this.hideSpinner();
			}
		}

		getCaption(url)
		{
			var idx = this.getIndex(url, this.images);
			return this.captions[idx];
		}

		getCredit(url)
		{
			var idx = this.getIndex(url, this.images);
			return this.credits[idx];
		}

		load()
		{
			if (this.options.preload || this.options.showThumbnails)
			{
				this.loadThumbnails();
			}
			else
			{
				this.busy = false;
			}

			if (!this.options.showInfo)
			{
				this.toggleInfo();
			}
		}
	}

	var instance;
	return function(options)
	{
		if (instance) return instance;
		else instance = new SlideshowSingleton(options);
		return instance;
	};
})();
