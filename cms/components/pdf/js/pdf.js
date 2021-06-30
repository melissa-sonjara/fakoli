/**
 * 
 */
var PDF = new Class({});

PDF.saveAsPDF = function()
{
	go('/action/pdf/generate?url=' + location.href);
};

PDF.saveAsPNG = function()
{
	go('/action/pdf/screenshot?url=' + location.href);
}