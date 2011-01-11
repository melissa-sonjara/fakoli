
var Slideshow = (function()
{
	var SlideshowSingleton = new Class(
	{
	    Implements: [Options, Events],
	    
	    options: 	{preload: true, showInfo: true, autoPlay: false},
	    busy: 		false,
		timer: 		null,
		playing: 	false,
		position: 	0,
		showing:	-1,
		timer:		Class.Empty,
		
		loadedThumbnails: [],
		loadedImages: [],
	
		slideshow: 		Class.Empty,
		viewport: 		Class.Empty,
		gallery:		Class.Empty,
		progress:		Class.Empty,
		bar:			Class.Empty,
		progressText:	Class.Empty,
		progressBorder:	Class.Empty,
		progressCount:	Class.Empty,
		strip:			Class.Empty,
		scroller:		Class.Empty,
		caption:		Class.Empty,
		credit:			Class.Empty,
		creditText:		Class.Empty,
		highlight:		Class.Empty,
		playButton:		Class.Empty,
		infoButton:		Class.Empty,
		spinner: 		Class.Empty,
		
		scrollLeftButton:	Class.Empty,
		scrollRightButton:	Class.Empty,
		
		zoomOverlay:	Class.Empty,
		zoomIn:			Class.Empty,
		zoomOut:		Class.Empty,		
		zoomSlider:		Class.Empty,
		
		zoomMax:	2,
		zoomMin:	0.0,
		zoomLevel:	0.0,
		
		top:	0,
		left:	0,
		
		highlightFX: Class.Empty,
		
		thumbnails: [],
		images: [],
		captions: [],
		credits: [],
		
		initialize: function(options)
		{
	    	this.setOptions(options);
	    	
			this.slideshow= $('slideshow');
			this.viewport = $('viewport');
			this.gallery = $('gallery');
			this.progress = $('progress');
			this.bar = $$('#progress .bar')[0];
			this.progressText = $$('#progress .text')[0];
			this.progressBorder = $$('#progress .border')[0];
			this.progressCount = $('progress_count');
			this.strip = $('thumbnail_strip');
			this.scroller = $('thumbnail_scroll');
			this.caption = $('caption');
			this.credit = $('credit');
			this.creditText = $('creditText');
			this.highlight = $('thumbnail_highlight');
			this.playButton = $('play_button');
			this.infoButton = $('info_button');
			
			this.scrollLeftButton = $('scroll_left');
			this.scrollRightButton = $('scroll_right');
			
			this.zoomOverlay = $('zoom_overlay');
			this.zoomIn = $('zoom_in');
			this.zoomOut = $('zoom_out');
								   
			this.scrollLeftButton.addEvent('mouseover', function() 	{ new Fx.Tween(this.scrollLeftButton, {duration: 400}).start('background-color', '#ccc', '#e8ac1c');}.bind(this));
			this.scrollLeftButton.addEvent('mouseout', 	function() 	{ new Fx.Tween(this.scrollLeftButton,  {duration: 400}).start('background-color', '#e8ac1c', '#ccc');}.bind(this));
			this.scrollLeftButton.addEvent('click', 	function() 	{ this.scrollThumbnails(-1); }.bind(this));
	
			this.scrollRightButton.addEvent('mouseover', function() {new Fx.Tween(this.scrollRightButton, {duration: 400}).start('background-color', '#ccc', '#e8ac1c');}.bind(this));
			this.scrollRightButton.addEvent('mouseout', function() {new Fx.Tween(this.scrollRightButton, {duration: 400}).start('background-color', '#e8ac1c', '#ccc');}.bind(this));
			this.scrollRightButton.addEvent('click', function() {this.scrollThumbnails(1); }.bind(this));
	
			this.highlightFX = new Fx.Morph('thumbnail_highlight', {duration: 500});
	
			this.gallery.addEvent('mouseover',	function()	{ if (!this.playing) this.zoomOverlay.setStyle('opacity', 1); }.bind(this));
			this.gallery.addEvent('mouseout', 	function()	{ this.zoomOverlay.setStyle('opacity', 0); }.bind(this));
			this.gallery.addEvent('wheelup',	function(e) { e = new Event(e).stop(); if (!this.playing) this.alterZoomLevel(-10); }.bind(this));
			this.gallery.addEvent('wheeldown', 	function(e) { e = new Event(e).stop(); if (!this.playing) this.alterZoomLevel(10); }.bind(this));
			
			this.gallery.addEvent('mousewheel', function(e) 
			{ 
				e = new Event(e).stop(); if (!this.playing) this.alterZoomLevel((e.wheel < 0) ? 10 : -10);
			}.bind(this));
			
			this.playButton.addEvent('click', function() { this.togglePlay(); }.bind(this));
			this.infoButton.addEvent('click', function() { this.toggleInfo(); }.bind(this));
			
			this.scroller.setStyles({'position': 'absolute'});
			
			this.zoomSlider = new Slider($('zoom_scale'), $('zoom_thumb'), { steps: 100, 
																	mode: 'vertical', 
																	onChange: function(step) {this.setZoomPercent(step); }.bind(this) 
																  }).set(100);
	
			this.zoomIn.addEvent('click', function() { this.alterZoomLevel(-10); }.bind(this));
			this.zoomOut.addEvent('click', function() { this.alterZoomLevel(10); }.bind(this));
	
			var top = this.gallery.getCoordinates().height / 2 - this.progress.getCoordinates().height / 2 + this.gallery.getCoordinates().top;
			var left =  this.gallery.getCoordinates().width / 2 - this.progress.getCoordinates().width / 2 + this.gallery.getCoordinates().left;

			if (this.options.preload)
			{
				this.progress.setStyles({'visibility': 'visible',
				'left': left,
				'top': top,
				'opacity': .75});
			}

			this.spinner = new Asset.image("/fakoli/slideshow/images/loading_animation.gif",
			{
				onload:	function()
				{
					this.spinner.injectInside(this.gallery);					
					this.spinner.setStyle('display', 'none');
				}.bind(this)
			});
		},
		
		loadThumbnails: function()
		{		
			this.progressText.innerHTML = "Loading thumbnails...";
			var slideshow = this;
			
			new Asset.images(slideshow.thumbnails, 
			{				
				onProgress: function(i)
				{
					var thumbW, thumbH, ratio;
					ratio = (this.width > this.height) ? 60 / this.width : 60 / this.height;
					
					this.setStyles({
						'position': 'relative',
						'opacity': .75,
						'width': (this.width * ratio),
						'height': (this.height * ratio)
						});
						
					
					slideshow.loadedThumbnails[slideshow.getIndex(this.src, slideshow.thumbnails)] = this;
					var percent = ((i + 1) * slideshow.progress.getStyle('width').toInt()) / slideshow.thumbnails.length;
					slideshow.bar.setStyle('width', percent);
					slideshow.progressCount.innerHTML = i+1 + " of " + slideshow.thumbnails.length;
					
				},
				
				onComplete: function()
				{
	
					
					slideshow.loadImages();
				}
			});	
		},
	
		loadImages: function()
		{
			this.progressText.innerHTML = "Loading images...";
			var slideshow = this;
			new Asset.images(slideshow.images, 
			{			
				onProgress: function(i)
				{
					this.setStyles({
								    'display': 'block',
									'position': 'absolute',
									'opacity': 0,
									'z-index': 1,
									'visibility': 'visible'
								});
	
					slideshow.gallery.adopt(this);				
					slideshow.positionImage(this);			
					var drag = new Drag.Move(this, 
					{
						preventDefault: true,
						onDrag:function(element, event) 
						{
							new Event(event).stop(); 
						}
					});
					
					slideshow.loadedImages[slideshow.getIndex(this.src, slideshow.images)] = this;
					var percent = ((i + 1) * slideshow.progress.getStyle('width').toInt()) /slideshow. thumbnails.length;
					slideshow.bar.setStyle('width', percent);
					slideshow.progressCount.innerHTML = i+1 + " of " + slideshow.thumbnails.length;
				},
	
				onComplete: function()
				{
					slideshow.progress.setStyle('visibility', 'hidden');
					
					slideshow.scroller.setStyle('width', slideshow.thumbnails.length * 100);
					slideshow.scroller.setStyle('left', 0);
					slideshow.highlight.setStyle('left', -100);
					
					slideshow.loadedThumbnails.each(function(image, i) {
						image.inject(slideshow.scroller);
						image.setStyles({'left': (i * 20),
										 'top': 10,
										 'vertical-align': 'middle',
										 'cursor': 'pointer'});
						image.addEvent('mouseover', function() { this.setStyle('opacity', 1); });
						image.addEvent('mouseout', function() { this.setStyle('opacity', .75); });
						image.addEvent('click', function() {this.clickImage(i); }.bind(slideshow));
					});
					
					slideshow.busy = false;
					if (slideshow.thumbnails.length > 0) slideshow.selectImage(0);
					if (slideshow.options.autoPlay)						
					{
						slideshow.togglePlay();
					}
				}
			});		
		},
	
		positionImage: function(img)
		{
			var gw, gh, ratioH, ratioW;
			gw = this.gallery.getCoordinates().width - 20;
			gh = this.gallery.getCoordinates().height - 20;
			
			if (img.naturalWidth == undefined) img.naturalWidth = img.width;
			if (img.naturalHeight == undefined) img.naturalHeight = img.height;
			
			var iw = img.naturalWidth || img.width;
			var ih = img.naturalHeight || img.height;
			
			ratioW = gw / iw;
			ratioH = gh / ih;
			
			this.ratio = (ratioW < ratioH) ? ratioW : ratioH;	
	
			var w = this.viewport.getCoordinates().width - 20;
			var h = this.viewport.getCoordinates().height - 20;
			
			var l = (w / 2) - (iw * this.ratio / 2) + 10;
			var t = (h / 2) - (ih * this.ratio / 2) + 10;
			
			img.setStyles({
							'left': l,
							'top': t,
							'width': iw * this.ratio,
							'height': ih * this.ratio
						});
			
			return this.ratio;
		},

		getZoomPercent: function()
		{
			var percent = 100 - (100 * (this.zoomLevel - this.zoomMin)) / (this.zoomMax - this.zoomMin);
			return percent;
		},
	
		alterZoomLevel: function(amount)
		{
			var percent = this.getZoomPercent() + amount;
			if (percent < 0) percent = 0;
			if (percent > 100) percent = 100;
			this.zoomSlider.set(percent);
		},
	
		setZoomPercent: function(percent)
		{
			if (this.showing == -1) return;
			
			var img = this.loadedImages[this.showing];
			
			if (!img) return;
			
			var x = img.getCoordinates().left - this.viewport.getCoordinates().left;
			var y = img.getCoordinates().top - this.viewport.getCoordinates().top;
			var vw = this.viewport.getCoordinates().width;
			var vh = this.viewport.getCoordinates().height;
					
			//console.debug("(%f, %f) %f x %f [%f x %f]", x, y, img.width, img.height, vw, vh);
	
			
			var zx = (this.viewport.getCoordinates().width / 2 - x);
			zx = zx / this.zoomLevel;
					
			var zy = (this.viewport.getCoordinates().height / 2 - y);
			zy = zy / this.zoomLevel;
	
			//console.debug("(%f, %f) : (%f, %f) %f", zoomX, zoomY, zx, zy, zoomLevel);
					
			this.zoomLevel = (this.zoomMax - this.zoomMin) * (100 - percent) / 100 + this.zoomMin;
	
			var iw = img.naturalWidth || img.width;
			var ih = img.naturalHeight || img.height;
			
			var w = iw * this.zoomLevel;
			var h = ih * this.zoomLevel;
			
			var offX = -(zx * this.zoomLevel) + this.viewport.getCoordinates().width / 2;
			var offY = -(zy * this.zoomLevel) + this.viewport.getCoordinates().height / 2;
			
			this.zoomX = zx;
			this.zoomY = zy;
			
			img.setStyles({'top': offY,
						   'left': offX,
						   'width': w,
						   'height': h});
		},
	
		selectImage: function(i)
		{
			if (this.busy) return;
			//alert("selectImage(" + i + ")");
			if (this.showing != -1)
			{
				new Fx.Tween(this.loadedImages[this.showing], {duration: 1500, transition: Fx.Transitions.linear}).start('opacity', 0);
			}	
			
			new Fx.Tween(this.loadedImages[i], {duration: 1500, transition: Fx.Transitions.linear}).start('opacity', 1);
			
			this.caption.innerHTML = this.captions[i];
			this.creditText.innerHTML = this.credits[i];
				
			this.showing = i;
		
	
			this.zoomMin = this.positionImage(this.loadedImages[i]);
			this.zoomLevel = this.zoomMin;
			this.zoomX = this.loadedImages[i].naturalWidth / 2;
			this.zoomY = this.loadedImages[i].naturalHeight / 2;
			this.zoomSlider.set(100);
	
			this.setZoomPercent(100);
			this.highlight.setStyle('visibility', 'visible');
			var l = this.loadedThumbnails[i].offsetLeft - 10;
			var w = this.loadedThumbnails[i].width + 20;
			this.highlightFX.start({
				'left': l,
				'width': w
				});
				
			var remaining = this.strip.offsetWidth - (l + this.scroller.offsetLeft);
			var offset = this.loadedThumbnails[this.showing].offsetLeft - 4;
			if (remaining < 100 || offset < -this.scroller.offsetLeft)
			{
				new Fx.Tween(this.scroller, {duration: 1000}).start('left', -offset);
			}
		},
	
		selectImageImmediate: function(i)
		{
			if (this.busy) return;
			if (this.showing != -1)
			{
				this.loadedImages[this.showing].setStyle('opacity', '0');
			}
			
			this.loadedImages[i].setStyle('opacity', 1);

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
				this.highlight.setStyle('visibility', 'visible');
				var l = this.loadedThumbnails[i].offsetLeft - 10;
				var w = this.loadedThumbnails[i].width + 20;
				this.highlightFX.start({
					'left': l,
					'width': w
				});
			
				var remaining = this.strip.offsetWidth - (l + this.scroller.offsetLeft);
				var offset = this.loadedThumbnails[this.showing].offsetLeft - 4;
				if (remaining < 100 || offset < -scroller.offsetLeft)
				{
					new Fx.Tween(this.scroller, {duration: 1000}).start('left', -offset);
				}
			}
		},
		
		clickImage: function(i)
		{
			if (!this.playing && !this.busy)
			{
				this.selectImage(i);
			}
		},		
	
		getIndex: function(txt, arr)
		{
			for(i = 0; i < arr.length; ++i)
			{
				if (arr[i] == txt)
				{
					return i;
				}
			}
			
			return -1;
		},
		
		scrollThumbnails: function(dir)
		{
			if (this.busy) return;
			this.position += dir;
			if (this.position <= 0) this.position = 0;
			else if (this.position > this.thumbnails.length - 4) this.position = this.thumbnails.length - 4;
			
			var offset = this.loadedThumbnails[this.position].offsetLeft;
			new Fx.Tween(this.scroller, {duration: 1000}).start('left', -offset);
		},
	
		togglePlay: function()
		{
			if (this.playing)
			{
				this.playing = false;
				this.playButton.setStyle('background-image', 'url(/fakoli/slideshow/images/play.gif)');
			}
			else
			{
				this.playing = true;
				this.playButton.setStyle('background-image', 'url(/fakoli/slideshow/images/stop.gif)');
				this.playNext();
			}
		},
	
		toggleInfo: function()
		{
			var shown = this.caption.getStyle('display');
			if (shown == "inline")
			{
				this.caption.setStyle('display', 'none');
				this.creditText.setStyle('display', 'none');
				this.infoButton.setStyle('background-image', 'url(/fakoli/slideshow/images/info_off.gif)');
			}
			else
			{
				this.caption.setStyle('display', 'inline');
				this.creditText.setStyle('display', 'inline');
				this.infoButton.setStyle('background-image', 'url(/fakoli/slideshow/images/info.gif)');
			}
		},
		
		playNext: function()
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
			
			this.timer = this.playNext.delay(10000);
		},
		
		selectImageFromURL: function(url)
		{
			idx = this.getIndex(url, this.images);
			this.selectImage(idx);
		},
	
		clear: function()
		{
			this.loadedImages.each(function(i) { if (i != null) i.setStyle('opacity', 0);});
		},
		
		showSpinner: function()
		{
			this.clear();
			
			this.spinner.setStyle('opacity', 1);
			
			var w = this.spinner.width;
			var h = this.spinner.height;
			var g = this.gallery.getCoordinates();
			var t = (g.height - h) / 2;
			var l = (g.width - w) / 2;
			this.spinner.setStyles(
			{

				'position': 'absolute',
				'z-index': 1,
				'top': t, 
				'left': l, 
				'display': 'block'
			});
		},
		
		hideSpinner: function()
		{
			this.spinner.setStyle('display', 'none');
		},
		
		showImage: function(url)
		{
			this.clear();
			
			idx = this.getIndex(url, this.images);
			//this.zoomSlider.refreshLayout();
			
			if (!this.options.preload)
			{
				if (!this.loadedImages[idx])
				{
					this.showSpinner();
					
					var slideshow = this;
					
					new Asset.image(url, 
					{
						onload: function() 
						{ 
							this.setStyles({
											'position': 'absolute',
											'opacity': 0,
											'z-index': 1
										});
			
							slideshow.positionImage(this);			
							this.inject(slideshow.gallery);		
							var drag = new Drag.Move(this, 
									{
										preventDefault: true,
										onDrag: function(element, event)
										{
											new Event(event).stop();
										}
									});
												
							slideshow.loadedImages[idx] = this; 
							slideshow.selectImageImmediate(idx); 
							slideshow.hideSpinner();
						}
					});
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
		},
	
		getCaption: function(url)
		{
			idx = this.getIndex(url, this.images);
			return this.captions[idx];
		},

		getCredit: function(url)
		{
			idx = this.getIndex(url, this.images);
			return this.credits[idx];
		},
		
		load: function()
		{
			if (this.options.preload)
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
	});
	
	var instance;
	return function(options)
	{
		if (instance) return instance;
		else instance = new SlideshowSingleton(options);
		return instance;
	};
})();

