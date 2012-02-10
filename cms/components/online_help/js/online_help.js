var OnlineHelp = new Class({});

OnlineHelp.help = function(book, page)
{
	 popup("/action/online_help/help?book=" + book + "&page=" + page, "help", 940, 500,  "toolbar=0,location=0,scrollbars=1,resizable=1");
};
