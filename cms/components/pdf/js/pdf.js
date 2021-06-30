/**
 * 
 */
var PDF = new Class({});

PDF.saveAsPDF = function()
{
	go('/action/pdf/generate?uri=' + location.href);
};

PDF.saveAsPNG = function()
{
	go('/action/pdf/screenshot?uri=' + location.href);
}