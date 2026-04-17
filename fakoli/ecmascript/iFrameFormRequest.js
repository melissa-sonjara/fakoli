/*
 * iFrameFormRequest — replaces MooTools-based iframe file-upload helper.
 *
 * Original author: Arian Stolwijk (MIT license)
 * Converted to plain ECMAScript.
 */

/**
 * Submits a form through a hidden iframe, enabling file uploads without a
 * full page reload and delivering the response body to an onComplete callback.
 */
class iFrameFormRequest
{
	constructor(formElmt, options)
	{
		this.options = Object.assign({
			onRequest:  null,
			onComplete: null,
			onFailure:  null
		}, options || {});

		this.frameId = 'f' + Math.floor(Math.random() * 99999);
		this.loading = false;

		this.formElmt = $el(formElmt);
		this.formElmt.setAttribute('target', this.frameId);

		var self = this;
		this.formElmt.addEventListener('submit', function()
		{
			if (self.options.onRequest)
			{
				var submitTest = self.options.onRequest();
				if (!submitTest) return false;
			}
			self.loading = true;
		});

		// Allow external code to set the loading flag
		this.formElmt.setLoading = function(val) { self.loading = val; };

		// Create the hidden iframe
		this.iframe = document.createElement('iframe');
		this.iframe.id = this.frameId;
		this.iframe.name = this.frameId;
		this.iframe.style.display = 'none';
		this.iframe.src = 'about:blank';

		this.iframe.addEventListener('load', function()
		{
			if (!self.loading) return;

			try
			{
				var doc = document.getElementById(self.frameId).contentWindow.document;
				if (doc)
				{
					if (doc.location.href == 'about:blank')
					{
						if (self.options.onFailure) self.options.onFailure();
					}
					else if (typeof self.options.onComplete == 'function')
					{
						// Strip <embed> tags from the response body
						var html = doc.body.innerHTML.replace(/<embed[^>]*>.*?<\/embed>/gi, '')
						                             .replace(/<embed[^>]*\/?>/gi, '');
						self.options.onComplete(html);
					}
				}
				else
				{
					if (self.options.onFailure) self.options.onFailure();
				}
			}
			catch (e)
			{
				if (self.options.onFailure) self.options.onFailure();
			}

			self.loading = false;
		});

		var doc = document.body || document.documentElement;
		doc.appendChild(this.iframe);
	}

	toElement()
	{
		return this.iframe;
	}
}

Element.prototype.iFrameFormRequest = function(options)
{
	this._iframeFormRequest = new iFrameFormRequest(this, options);
	return this._iframeFormRequest;
};