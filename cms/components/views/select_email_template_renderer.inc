<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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
 * SelectFieldViewSourceRenderer: Renders the specified list of options as a dropdown 
 * select box with a View button to the right which would display specified popup
 * preview.
 * 
 * @param obj $form the AutoForm object, passed by reference
 * 
 * @param string $field the name of the field defined in the dataitem class object 
 * 		   in the datamodel folder whose value can be changed by the select 
 * 		   item list box. This field name should match the field name in the 
 * 		   database.
 * 
 * @param string $label presented on the form to the user to describe the list box
 * 
 * @param array options defined in the dataitem class object that contains the 
 * 		   option ids and labels for the list box 
 * 		   For example, for action_type, create array ActionTypes as follows:
 * 			  actionTypes = array (
 * 					1	=>	"Introduction",
 *					2	=>	"Investigation",
 *					3	=>	"Formal Assessment"
 * 					)
 * 
 * @param string $popupUrl url of the source preview 
 * 
 * Calling script creates an instance as follows:
 * $myFieldSelect = new SelectFieldViewSourceRenderer($form, "field name", "Label", options array, 
 *      popupUrl);
 *      
 * calling script must call writeScript below the call to $script .= $form->writeScript:
 * $script .= $myFieldSelect->writeScript();
 *
 */

require_once  $config['homedir']."/framework/field_renderers/select_field_renderer.inc";

class SelectEmailTemplateRenderer extends SelectFieldRenderer
{
	var $popupUrl; // path of popup that display the source/preview
	var $buttonLabel; // defaults to field label
	var $field;
	
	function SelectEmailTemplateRenderer(&$form, $field, $label, $options, $popupUrl)
	{
		$onChange = "selChanged_{$field}";
		$this->field = $field;
		$this->popupUrl = $popupUrl . "?{$field}=";
		
		// show none if none - overrides SelectFieldRenderer code that
		// adds options when there are none
		if ($options == null)
			$options[0] = "";
		
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
		$this->buttonLabel = $label;
	}	
	
	function renderScript($field)
	{
		$fn = "{$this->parent->id}_$field";
		
?>
<script type="text/javascript">
function <?echo $fn?>_preview()
{
	var elt = $('<? echo $field ?>');
	
	if (elt.value)
	{
		popup('<? echo $this->popupUrl ?>' + elt.value, '', 750, 640);
	}
}
</script>
<?		
	}
	
	function writeScript()
	{
		$fn = "selChanged_$this->field";
		
		ob_start();
?>
<script type="text/javascript">
function <? echo $fn ?>(elt)
{
  	var button = $('view_<? echo $this->field ?>_button');
   	var cntrl = $('message');
	var fragmentURL =  "/email_template_fragment?template_id=";

   	if (elt.value > 0)
   	{
		button.disabled = "";
		cntrl.innerHTML = httpRequest(fragmentURL + elt.value);
	}
	else
		button.disabled = "disabled";
}

window.addEvent('load', function()
{
	var elt = $('<? echo $this->field ?>');
	<? echo $fn ?>(elt);	
});

	</script>
	<?
	$script .= ob_get_contents();
	ob_end_clean();
	
	return $script;		
	}

	function _endField($field)
	{
		$name = "view_{$field}_button";
		echo "&nbsp;<button id=\"$name\" name=\"$name\" onclick=\"{$this->parent->id}_{$field}_preview(); return false;\" class=\"button\">View {$this->buttonLabel}</button>";
		
		parent::_endField($field);
	}
} // end class SelectFieldViewSourceRenderer

?>