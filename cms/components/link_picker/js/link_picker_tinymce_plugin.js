tinymce.PluginManager.add('linkpicker', function(editor, url) {
    // Add a button that opens a window
    editor.addButton('linkpicker', {
    	title: 'Insert Link',
        image: '/fakoli/tinymce/skins/fakoli/img/linkpicker.png',
        onclick: function() {
        	linkPicker(editor);
        }
    });
});
