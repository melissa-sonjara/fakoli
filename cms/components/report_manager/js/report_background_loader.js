var ReportBackgroundLoader = new Class(
{
	element: Class.Empty,
	
	initialize: function(element)
	{
		this.element = document.id(element);
	},
	
	loadFromID: function(report_id, excel)
	{
		handler = "/action/report_manager/generate_report?report_id=" + report_id;
		if (excel)
		{
			handler += "&__excel=" + excel;
		}
		
		var request;
		
		if (Browser.ie)
		{
			request = new Request(
			{
				evalScripts: function(scripts)  {request.scripts = scripts; return false;},
				evalResponse: false,
				method: 'get', 
				url: handler, 
				onSuccess: function(html) 
				{ 
					this.element.set('text', '');
					this.element.set('html', html);
					$exec(request.scripts);
				}.bind(this)
			});
		}
		else
		{
			request = new Request.HTML(
			{
				evalScripts: false,
				evalResponse: false,
				method: 'get', 
				url: handler, 
				onSuccess: function(tree, elements, html, script) 
				{ 
					this.element.set('text', '');
					this.element.set('html', html);
					$exec(script);
				}.bind(this)
			});
		}
		request.send();

	},
	
	loadFromRequest: function(requestData, excel)
	{
		var handler = new URI("/action/report_manager/generate_report");
		
		
		var target = new URI().getData("target");
		if (target) handler.setData('target', target);
		
		if (excel)
		{
			handler.setData("?__excel=", excel);
		}

		var request;
		
		if (Browser.ie)
		{
			request = new Request(
			{
				evalScripts: function(scripts)  {request.scripts = scripts; return false;},
				evalResponse: false,
				method: 'post', 
				url: handler,
				data: requestData,
				onSuccess: function(html) 
				{ 
					this.element.set('text', '');
					this.element.set('html', html);
					$exec(request.scripts);
				}.bind(this)
			});
		}
		else
		{
			request = new Request.HTML(
			{
				evalScripts: false,
				evalResponse: false,
				method: 'post', 
				url: handler,
				data: requestData,
				onSuccess: function(tree, elements, html, script) 
				{ 
					this.element.set('text', '');
					this.element.set('html', html);
					$exec(script);
				}.bind(this)
			});
		}
		request.send();
	}
	
});

