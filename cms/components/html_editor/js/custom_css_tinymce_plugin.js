tinymce.PluginManager.add('custom_css', function(editor, url) {
    // Add a button that opens a window
    editor.addButton('custom_css', {
    	title: 'Edit Custom CSS',
        image: '/components/html_editor/images/custom_css.png',
        onclick: function() {
        	new CustomCSSManager().showCSSEditor(editor);
        }
    });

    editor.addMenuItem('custom_css', {
    	text: 'Edit Custom CSS',
        image: '/components/html_editor/images/custom_css.png',
        onclick: function() {
        	new CustomCSSManager().showCSSEditor(editor);
        }
    });

});