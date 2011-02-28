<?php

require_once realpath(dirname(__FILE__)."/auto_form.inc");

/**
 * CompositeAutoForm is a container for situations where multiple AutoForms need to
 * be managed on the same page within a single outer form.
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
	 * @param $form the form to be added
	 */
	function addForm($form)
	{
		$form->makeSubordinate();
		$this->forms[] = $form;
		$form->getValidationEngine()->generateScript = false;
	}

	/**
	 * Write script for all the sub-forms.
	 */
	function writeScript()
	{
		$script = "";
		foreach($this->forms as $form)
		{
			$script .= $form->writeScript();
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
	 * Draw the sub-forms, with a combined single set of submission buttons.
	 */
	function drawForm()
	{
		echo "<form id='composite_auto_form' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
		echo " onsubmit='return onCompositeFormSubmit(this);'";
		echo ">\n";

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
			echo "<input type='submit' class='{$this->buttonCSS}' name='submit' value='$submitLabel'/>";
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
			
			if (!$form->save()) $valid = false;
		}
		
		return $valid;
	}
		
}
