<?php
require_once "framework/field_renderers/password_field_renderer.inc";

Fakoli::usingFeature("auto_form");

class PasswordComplexityFieldRenderer extends PasswordFieldRenderer
{
	
	function PasswordComplexityFieldRenderer(&$parent)
	{
		$this->PasswordFieldRenderer($parent);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		parent::addValidatorsToForm($field, $required);
		$this->parent->validator->add(new PasswordComplexityValidator($field, $title, $this->parent->id));
	}
	
	function renderScript($field)
	{
		$minLength = Settings::getValue("login", "minimum_password_length");
		$maxLength = Settings::getValue("login", "maximum_password_length");
		if (!$minLength) $minLength = "0";
		if (!$maxLength) $maxLength = "0";
		
		$requireDigits = Settings::getValue("login", "require_digits") ? 'true' : 'false';
		$requireUppercase = Settings::getValue("login", "require_uppercase") ? 'true' : 'false';
		$requireSpecial = Settings::getValue("login", "require_special") ? 'true' : 'false';
		$id = "{$this->parent->id}_{$field}";
		//		<script type='text/javascript' src='/components/login/js/password_complexity_verifier.js'></script>
?>
<script type='text/javascript'>
window.addEvent('domready', function()
{
	new PasswordComplexityVerifier('<?echo $id?>', '<?echo $id?>_complexity_panel',
	{
		minimum_password_length:  	<?echo $minLength?>,
		maximum_password_length:	<?echo $maxLength?>,
		require_digits:				<?echo $requireDigits?>,
		require_uppercase:			<?echo $requireUppercase?>,
		require_special:			<?echo $requireSpecial?>
	});
});	
</script>
<?
		if (!$this->_includedPasswordScript)
		{
			?>
	<script type='text/javascript'>
	<!--
	function <?echo $this->parent->id?>_<?echo $field?>_showPassword()
	{
		var link = document.getElementById("<?echo $this->parent->id?>_<?echo $field?>_link");
		var block = document.getElementById("<?echo $this->parent->id?>_<?echo $field?>_block");
		document.forms['<?echo $this->parent->id ?>'].<?echo $this->parent->id ?>_<?echo $field?>_reset_password.value = 1;
		
		link.style.display = "none";
		block.style.display = "block";
	}
	//-->
	</script>
	<?
			$this->_includedPasswordScript = true;
		}		
	}

	function renderPasswordBlock($field, $display = "block")
	{
		$enter = $this->parent->passwordEnterLabel;
		$confirm = $this->parent->passwordConfirmLabel;
		$size = $this->numeric ? "6" : "20";
		$onKeyPress = $this->numeric ? " onkeypress='return maskInput(event, $allowneg);'" : ""
		?>
		 	<div  id="<?echo $this->parent->id ?>_<?echo $field?>_block" class='password_block' style="padding: 0px; display: <?echo $display ?>">
				 <table border="0" cellspacing="0" cellpadding="0">
				  <tr>
	  				<td style="vertical-align: top; text-align: left"><?echo $enter?>&nbsp;</td>
	  				<td style="padding-bottom: 2px"><input type="password" autocomplete="off" name="<?echo $field?>" id="<?echo $this->parent->id?>_<?echo $field?>" size="<?echo $size?>"<?echo $onKeyPress?> value=""/></td>
	 			  </tr>
	 			  <tr>
	  				<td style="vertical-align: top; text-align: left"><?echo $confirm?>&nbsp;</td>
	  				<td><input type="password" name="<?echo $field?>_confirm" id="<?echo $this->parent->id?>_<?echo $field?>_confirm" size="<?echo $size?>"<?echo $onKeyPress?> value=""/></td>
	 			  </tr>
	 			 </table>		
	 			 <div id='<?echo $this->parent->id?>_<?echo $field?>_complexity_panel' class='password_complexity_panel'></div>
	 		</div>
		<?
	}
}

class PasswordComplexityValidator extends PasswordValidator
{
	function PasswordComplexityValidator($field, $title, $form_id = "", $empty = "")
	{
		$this->PasswordValidator($field, $title, $form_id, $empty);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
	
		$pswField = "{$this->form_id}_{$this->field}";
		$pswBlock = "{$pswField}_block";
		/*
	var block = document.getElementById("$pswBlock");
	if (block.style.display != "none" && form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}*/

	$script = <<<ENDSCRIPT
	
	var control = document.id("$pswField");
	if (control.hasClass("password_fail"))
	{
		alert("Please make sure {$this->title} conforms to the password complexity rules");
		return false;
	}
										
ENDSCRIPT;
	
		return $script;
	}
	
	function validate()
	{
		global $_POST;
	
		if ($this->readOnly) return "";
	
		$reset = $_POST["{$this->form_id}_reset_password"];
	
		if ($reset == 1)
		{
			if (!isset($_POST[$this->field]) || $_POST[$this->field] === $this->empty)
			{
				return "{$this->title} is a required field.";
			}
			else
			{
				$password = $_POST[$this->field];
				
				$minLength = Settings::getValue("login", "minimum_password_length");
				$maxLength = Settings::getValue("login", "maximum_password_length");
				
				$requireDigits = Settings::getValue("login", "require_digits");
				$requireUppercase = Settings::getValue("login", "require_uppercase");
				$requireSpecial = Settings::getValue("login", "require_special");
				
				$passLength = false;
					
				if ($minLength == 0 || strlen(password) >= $minLength)
				{
					if ($minLength == 0 || strlen($password) <= $maxLength)
					{
						$passLength = true;
					}
				}
					
				$passDigits = false;
					
				if ($requireDigits)
				{
					if (preg_match("/\d/", $password)) 
					{
						$passDigits = true;
					}
				}
				else
				{
					$passDigits = true;
				}
					
				$passUppercase = false;
					
				if ($requireUppercase)
				{
					if (preg_match("/[A-Z]/", $password) && preg_match("/[a-z]/", $password))
					{
						$passUppercase = true;
					}
				}
				else
				{
					$passUppercase = true;
				}
					
				$passSpecial = false;
					
				if ($requireSpecial)
				{
					if (preg_match("/[^A-Za-z0-9]/", $password))
					{
						$passSpecial = true;
					}
				}
				else
				{
					$passSpecial = true;
				}
				
				return ($passLength && $passUppercase && $passDigits && $passSpecial) ? "" : "Please ensure that {$this->title} conforms to the password complexity rules";
			}
		}
	}
}