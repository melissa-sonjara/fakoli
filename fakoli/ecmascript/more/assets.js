/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above
 copyright holders shall not be used in advertising or otherwise
 to promote the sale, use or other dealings in this Software
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 * Utilities for dynamically loading JavaScript, CSS, and image assets.
 */
var Asset = {

	/**
	 * Inject a <script> tag for the given source URL.
	 * Won't re-inject if a script with the same src already exists.
	 * @param {string} source
	 * @param {Object} [options]  id, onLoad, onError, document
	 * @returns {HTMLScriptElement}
	 */
	javascript: function(source, options)
	{
		options = options || {};
		var doc = options.document || document;

		var existing = doc.querySelector('script[src="' + source + '"]');
		if (existing) return existing;

		var script = doc.createElement('script');
		script.type = 'text/javascript';
		script.src = source;
		if (options.id) script.id = options.id;
		if (options.onLoad) script.addEventListener('load', options.onLoad);
		if (options.onError) script.addEventListener('error', options.onError);

		(doc.head || doc.body).appendChild(script);
		return script;
	},

	/**
	 * Inject a <link rel="stylesheet"> for the given source URL.
	 * Won't re-inject if a link with the same href already exists.
	 * @param {string} source
	 * @param {Object} [options]  id, media, onLoad, document
	 * @returns {HTMLLinkElement}
	 */
	css: function(source, options)
	{
		options = options || {};
		var doc = options.document || document;

		var existing = doc.querySelector('link[href="' + source + '"]');
		if (existing) return existing;

		var link = doc.createElement('link');
		link.rel = 'stylesheet';
		link.type = 'text/css';
		link.media = options.media || 'screen';
		link.href = source;
		if (options.id) link.id = options.id;

		(doc.head || doc.body).appendChild(link);

		if (options.onLoad)
		{
			var loaded = false, retries = 0, timeout = options.timeout || 3000;
			var check = function()
			{
				var sheets = document.styleSheets;
				for (var i = 0; i < sheets.length; i++)
				{
					var owner = sheets[i].ownerNode || sheets[i].owningElement;
					if (owner && owner === link)
					{
						loaded = true;
						options.onLoad.call(link);
						return;
					}
				}
				retries++;
				if (!loaded && retries < timeout / 50) setTimeout(check, 50);
			};
			setTimeout(check, 0);
		}

		return link;
	},

	/**
	 * Preload an image.
	 * @param {string} source
	 * @param {Object} [options]  onLoad, onError, id, width, height
	 * @returns {HTMLImageElement}
	 */
	image: function(source, options)
	{
		options = options || {};
		var img = new Image();
		if (options.id)     img.id     = options.id;
		if (options.width)  img.width  = options.width;
		if (options.height) img.height = options.height;

		if (options.onLoad)  img.addEventListener('load',  options.onLoad);
		if (options.onError) img.addEventListener('error', options.onError);

		img.src = source;
		return img;
	},

	/**
	 * Preload multiple images, firing callbacks as each loads.
	 * @param {string[]} sources
	 * @param {Object} [options]  onComplete, onProgress, onError, properties
	 * @returns {HTMLImageElement[]}
	 */
	images: function(sources, options)
	{
		sources = Array.isArray(sources) ? sources : [sources];
		options = Object.assign({ onComplete: function(){}, onProgress: function(){}, onError: function(){} }, options || {});

		var counter = 0;
		var total   = sources.length;

		return sources.map(function(source, index)
		{
			return Asset.image(source, Object.assign({}, options.properties || {}, {
				onLoad: function()
				{
					counter++;
					options.onProgress.call(this, counter, index, source);
					if (counter === total) options.onComplete();
				},
				onError: function()
				{
					counter++;
					options.onError.call(this, counter, index, source);
					if (counter === total) options.onComplete();
				}
			}));
		});
	}

};
