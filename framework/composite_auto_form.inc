<?php
/**\addtogroup autoform
 * @{
 */
require_once realpath(dirname(__FILE__)."/auto_form.inc");

/**
 * CompositeAutoForm is a container for situations where multiple AutoForms need to
 * be managed on the same page within a single outer form.
 * 
 * To create a composite form, create an instance of
 * CompositeAutoForm and add to it all the subforms
 * (instances of SubordinateAutoForm) to appear on that page.
 * Some subforms may need special handling and so you will
 * need to create a child class of SubordinateAutoForm.  
 * 
 * Example:
 * $compositeForm = new CompositeAutoForm("POST", "", "my_form_id");
 * 
 * $compositeForm = new CompositeAutoForm("POST", "", "parts_order_address_form");
 * $shippingPanel = new OrderAddressForm($compositeForm, "shipping", "Shipping Address", $shippingAddress, "ship_address_id", $orderShip);
 * $billingPanel = new OrderAddressForm($compositeForm, "billing", "Billing Address", $billingAddress, "bill_address_id", $order);
 * 
 * Where OrderAddressForm is a child of class SubordinateAutoForm.
 *  
 * @author andy
 *
 */
class CompositeAutoForm
{
	var $forms;
	var $buttons = array();
	var $method;
	var $action;
	var $containerClass;
	var $buttonAlignment = "left";
	var $preset = array();
	var $readOnlyForm;  			///< Specifies whether the entire form is read-only
	var $onSaveComplete = null;		///< Callback event handler that is fired after all the subordinate AutoForms have finished saving data to the database.
	
	
	/**
	 * Creates a new CompositeAutoForm
	 * @param string $method the HTTP method to use
	 * @param string $action the URL to submit to
	 * @param string $containerClass CSS class to use for the container for each sub-form
	 */
	function CompositeAutoForm($method = "POST", $action = "", $containerClass = "")
	{
		global $auto_form_defaults;

		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}
		
		$this->forms = array();
		$this->method = $method;
		$this->action = $action;
		$this->containerClass = $containerClass;	
	}
	
	/**
	 * Add a sub-form to this form.
	 * @param AutoForm $form the form to be added
	 */
	function addForm($form, $enableValidation = false)
	{
		$form->makeSubordinate();
		$this->forms[] = $form;
		$form->getValidationEngine()->generateScript = $enableValidation;
	}

	/**
	 * Write script for all the sub-forms.
	 */
	function writeScript()
	{
		$script = "";
		foreach($this->forms as $form)
		{
			$s = $form->writeScript();
			if ($form->getValidationEngine()->generateScript)
			{
				// Remap form control ids
				$s= preg_replace("/\\bform\\[(['\"])([^'\"]*?)['\"]\\]/", "form[$1{$form->id}__$2$1]", $s);
			}
			$script .= $s;
		}

		ob_start();
?>
<script type="text/javascript">
function onCompositeFormSubmit(form)
{
<?
		foreach($this->forms as $form)
		{
?>
	if (!onSubmit<?echo $form->id?>(form)) return false;
<?
		}
	?>
	return true;
}
</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}
	
	/**
	 * Array of fields to be ignored when determining if a 
	 * record is empty in cases where empty records should not
	 * trigger an error.
	 * 
	 * For example:
	 * A spreadsheet form has the fields "sort_order" preset.
	 * The user does not complete any values in a row. If
	 * the subordinate form has allowEmpty set to true, 
	 * we don't want the value in the sort_order field to 
	 * trigger an error message.
	 */
	function preset()
	{
		foreach(func_get_args() as $preset)
		{
			$this->presetFields[$preset] = true;
		}
	}	
	
	/**
	 * Draw the sub-forms, with a combined single set of submission buttons.
	 */
	function drawForm()
	{
		if ($this->readOnlyForm) return $this->drawReadOnly();
		
		echo "<form id='composite_auto_form' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
		echo " onsubmit='return onCompositeFormSubmit(this);'";
		echo ">\n";

		if ($this->buttons_at_top) 
		{
			$this->drawButtons();
		}
		
		foreach($this->forms as $form)
		{
			ob_start();
			$form->drawForm();
			$output = ob_get_contents();
			ob_end_clean();
			
			$output = preg_replace("/\\bname=(['\"])([^'\"]*?)['\"]/", "name=$1{$form->id}__$2$1", $output);
			
?>
		<div id="<?echo $form->id?>_container" class="<?echo $this->containerClass?>">
<?
			echo $output;
?>
		</div>
<?
		}
		
		$this->drawButtons();
		
		echo "</form>\n";
	}	
	
	/**
	 * Draw the read-only sub-forms.
	 */
	function drawReadOnly()
	{
		foreach($this->forms as $form)
		{
			ob_start();
			$form->drawReadOnly();
			$output = ob_get_contents();
			ob_end_clean();
			
			$output = preg_replace("/\\bname=(['\"])([^'\"]*?)['\"]/", "name=$1{$form->id}__$2$1", $output);
			
?>
		<div id="<?echo $form->id?>_container" class="<?echo $this->containerClass?>">
<?
			echo $output;
?>

		</div>
<?
		}
		
		echo "<div style='clear:both'>&nbsp;</div><br/>";
	}
	
	/**
	 * Adds a custom button to the form.
	 *
	 * @param string $text the button label text
	 * @param string $url the URL to handle the button press
	 * @param string $confirm optional confirmation message
	 * @param boolean $isScript true if the url is javascript code to execute, 
	 * 							false if it is a URL to redirect to
	 */
	function button($text, $url, $confirm = null, $isScript = false)
	{
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm, 'isScript' => $isScript);
	}
	
	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
	
		$submitLabel = $this->submitLabel;
		$obj = $this->forms[0]->getData();
		
		if ($submitLabel == "")
		{
			$submitLabel = "Save ".pluralize($obj->prettifyClassName());
		}

		echo "<div style='clear: both; text-align: {$this->buttonAlignment}'><br/>";
		if ($this->useLinkSubmit)
		{
			echo "<a class='{$this->buttonCSS}' name='submit' onclick='if (onSubmit{$this->id}(document.forms.{$this->id})) return document.forms.{$this->id}.submit(); else return false;'>$submitLabel</a>";
		}
		else
		{
			echo "<input type='submit' class='{$this->buttonCSS}' value='$submitLabel'/>";
		}
		
		foreach($this->buttons as $button)
		{
			$url = ($button['isScript']) ? $button['url'] : "go('{$button['url']}');";
			
			if ($button['confirm'])
			{
				$link = "if (confirm('".jsSafe($button['confirm'])."')) $url; return false;";
			}
			else
			{
				$link = "$url; return false;";
			}

			echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type='button' class='{$this->buttonCSS}' onclick=\"$link\" value=\"{$button['text']}\"/>";
		}
		
		echo "</div>";
	}
	
	
	function save()
	{
		global $method;
		
		// Do magic
		
		$valid = true;
		
		foreach($this->forms as $form)
		{
			foreach($_POST as $name => $value)
			{
				if (strpos($name, "__") === false)
				{
					unset($_POST[$name]);
				}
			}			
			
			$prefix = $form->id."__";
			$len = strlen($prefix);
			
			foreach($_POST as $name => $value)
			{
				if (!strncmp($name, $prefix, $len))
				{
					$_POST[substr($name, $len)] = $value;
				}
			}
			
			foreach($_FILES as $name => $value)
			{
				if (strpos($name, "__") === false)
				{
					unset($_FILES[$name]);
				}
			}
			
			$prefix = $form->id."__";
			$len = strlen($prefix);
			
			foreach($_FILES as $name => $value)
			{
				if (!strncmp($name, $prefix, $len))
				{
					$_FILES[substr($name, $len)] = $value;
				}
			}
			
			if (!$form->save()) 
			{
				$valid = false;
			}
		}
		
		if ($valid)
		{
			// onComplete event is fired once all processing has been completed
			
			if ($this->onSaveComplete)
			{
				call_user_func_array($this->onSaveComplete, array($this));
			}
		}
		
		return $valid;
	}
		
}

/*
 * Creates a form that can be a subordinate to
 * a CompositeAutoForm.
 * 
 * @param parent - the instance of CompositeAutoForm
 * this form is subordinate to
 * 
 * @param form - the instance of AutoForm with all
 * any custom settings needed (aliases, hidden fields, etc).
 * 
 * @allowEmpty - whether a completely empty form, that
 * will not be saved, should be considered an error
 * condition. For spreadsheet forms, an empty row should
 * not report an error.
 * 
 */
class SubordinateAutoForm extends AutoForm
{
	var $id;
	var $form;
	var $parent;
	var $msg;
	var $allowEmpty; // whether we want to show an error message if entire subform is empty
	
	function SubordinateAutoForm($parent, $form, $allowEmpty  = false)
	{
		$this->parent = $parent;
		$this->form = $form;
		$this->id = $form->id;
		$this->allowEmpty = $allowEmpty;
		
		$this->parent->addForm($this);	
	}
	
	/*
	 * For subordinates that require special handling,
	 * create a child class of SubordinateAutoForm
	 * and set the form's special characteristics here,
	 * then add to its Composite instance.
	 */
	function createForm()
	{
		$this->form = new AutoForm($this->obj, "POST", "", $this->id);

		// set hidden fields, aliases, special renderers etc.
		$this->parent->addForm($this);
	}
	
	/**
	 * Retrieve the validation engine for this AutoForm.
	 */
	function getValidationEngine()
	{
		return $this->form->validator;
	}
	
	function getData()
	{
		return $this->form->getData();
	}
		
	function setDataSet($params)
	{
		$this->form->setDataSet($params);
	}
	
	function writeScript()
	{
		return $this->form->writeScript();
	}
	
	function drawForm()
	{
		$this->form->drawForm();
	}
	
	function drawReadOnly()
	{
		$this->form->drawReadOnly();
	}
	
	function makeSubordinate()
	{
		$this->form->makeSubordinate();
	}
	
	/*
	 * If an empty subform should not produce an
	 * error message then return true to the 
	 * parent composite form.
	 */
	function save()
	{
		$rtn = $this->form->save();				

		if($rtn == false && $this->allowEmpty && $this->isEmpty())
		{
			$rtn = true;
		}
		else
			$this->msg = $this->form->msg;
		
		return $rtn;
	}

	/*
	 * If we don't want to show an error message
	 * when an entire subform is empty then
	 * check if empty. This is the case for
	 * a spreadsheet that provides multiple empty
	 * rows and we won't save if empty but don't 
	 * consider a blank row an error.
	 */
	function isEmpty()
	{
		$empty = true;
		$obj =& $this->form->getData();	
		$pk = $obj->getPrimaryKey();
		
		$fields = $obj->getFields();
		$filter = $obj->getFilter();
		
		foreach($fields as $field => $type)
		{
			if ($field != $pk && !array_key_exists($field, $this->form->hidden) && !($filter && $filter->isExcluded($field)) && !array_key_exists($field, $this->parent->presetFields))
			{
				$value = $obj->$field;
				if($value)
				{
					$empty = false;
					break;
				}
			}
		}
		return $empty;
	}
}
/**@}*/