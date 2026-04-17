class ReportBackgroundLoader
{
	constructor(element)
	{
		this.element = document.getElementById(element);
	}

	loadFromID(report_id, excel)
	{
		var handler = "/action/report_manager/generate_report?report_id=" + report_id;
		if (excel)
		{
			handler += "&__excel=" + excel;
		}

		fetch(handler)
			.then(r => r.text())
			.then(html =>
			{
				this.element.textContent = '';
				this.element.innerHTML = html;
			});
	}

	loadFromRequest(requestData, excel)
	{
		var url = new URL("/action/report_manager/generate_report", window.location.origin);

		var currentParams = new URLSearchParams(window.location.search);
		var target = currentParams.get("target");
		if (target) url.searchParams.set('target', target);

		if (excel)
		{
			url.searchParams.set('__excel', excel);
		}

		fetch(url.toString(),
		{
			method: 'post',
			body: requestData
		})
			.then(r => r.text())
			.then(html =>
			{
				this.element.textContent = '';
				this.element.innerHTML = html;
			});
	}
}
