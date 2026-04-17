tinymce.PluginManager.add('videopicker', function(editor, url)
{
	editor.addButton('videopicker',
	{
		title: 'Insert Video',
		image: '/fakoli/tinymce/skins/fakoli/img/insertvideo.png',
		onclick: function()
		{
			videoPicker(editor);
		}
	});

	editor.addMenuItem('videopicker',
	{
		text:  'Insert Video',
		icon:  'videopicker',
		image: '/fakoli/tinymce/skins/fakoli/img/insertvideo.png',
		onclick: function()
		{
			videoPicker(editor);
		}
	});
});
