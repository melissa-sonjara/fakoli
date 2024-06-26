<?php
/**\addtogroup terms_and_conditions
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class TermsAndConditionsFieldRenderer extends FieldRenderer
{
	var $containerFormID = null;
	
	function TermsAndConditionsFieldRenderer(&$parent, $field)
	{
		global $isAdmin;
		
		$this->FieldRenderer($parent);
		if (!$isAdmin && Settings::getValue("terms_and_conditions", "require_acceptance"))
		{
			$this->parent->onSubmitHandler = "TermsAndConditions.showTerms";
		}
		$this->parent->hide($field);
		$obj = $this->parent->data;
		if ($obj->get($field) == '') $obj->set($field, 0);
	}
	
	function renderScript($field)
	{
		if (!Settings::getValue("terms_and_conditions", "require_acceptance")) return;
		
		$formId = ($this->containerFormID) ? $this->containerFormID : $this->parent->id;
		$id = $this->parent->id."_".$field;
		$title = Settings::getValue("terms_and_conditions", "title");
?>
<script type="text/javascript">
TermsAndConditions.formId = "<?echo $formId?>";
TermsAndConditions.id = "<?echo $id?>";
TermsAndConditions.title = "<?echo $title?>";
</script>
<?
	}
			
	function renderField($field)
	{
	}
}


// Register the field renderer
AutoForm::registerFieldRendererClass(TermsAndConditions, TermsAndConditionsFieldRenderer);
/**@}*/?>