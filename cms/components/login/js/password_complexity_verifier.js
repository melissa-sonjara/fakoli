var PasswordComplexityVerifier = new Class(
{
	Implements: Options,
	
	options: 
	{
		minimum_password_length:  	6,
		maximum_password_length:	12,
		require_digits:				true,
		require_uppercase:			true,
		require_special:			true,
		
		require_digits_message:		"at least one numeric character",
		require_uppercase_message:	"at least one uppercase and one lowercase character",
		require_special_message:	"at least one special (punctuation or whitespace) character"
	},

	lengthMessage: "",
	
	control: 	Class.Empty,
 	panel:		Class.Empty,
 	
 	passLength:		false,
 	passDigits:		false,
 	passUppercase: 	false,
 	passSpecial:	false,
 	
 	initialize: function(control, panel, options)
 	{
 		this.control = document.id(control);
 		this.panel = document.id(panel);
 		this.setOptions(options);
 		
 		if (this.options.mininum_password_length == 0 && this.options.maximum_password_length > 0)
 		{
 			this.lengthMessage = "up to " + this.options.maximum_password_length + " characters";
 		}
 		else if (this.options.maximum_password_length == 0 && this.options.minimum_password_length > 0)
 		{
 			this.lengthMessage = "at least " + this.options.minimum_password_length + " characters";
 		}
 		else if (this.options.maximum_password_length > 0 && this.options.minimum_password_length > 0)
 		{
 			this.lengthMessage = "between " + this.options.minimum_password_length + " and " + this.options.maximum_password_length + " characters";
 		}
 		
 		this.updatePanel();
 		this.control.addEvent('keyup', function(event) { this.checkPassword();}.bind(this));
 	},
 	
 	getPassFail: function(pass)
 	{
 		return (pass) ? "pass" : "fail";
 	},
 	
 	updatePanel: function()
 	{
 		var msg = "";
 		
 		if (this.options.minimum_password_length || this.options.maximum_password_length)
 		{
 			msg += "<span class='" + this.getPassFail(this.passLength) + "'>" + this.lengthMessage + "</span>";
 		}
 		
 		if (this.options.require_digits)
 		{
 			msg += "<span class='" + this.getPassFail(this.passDigits) + "'>" + this.options.require_digits_message + "</span>";
 		}
 		
 		if (this.options.require_uppercase)
 		{
 			msg += "<span class='" + this.getPassFail(this.passUppercase) + "'>" + this.options.require_uppercase_message + "</span>";
 		} 	
 		
 		if (this.options.require_special)
 		{
 			msg += "<span class='" + this.getPassFail(this.passSpecial) + "'>" + this.options.require_special_message + "</span>";
 		}
 		
 		this.panel.set('html', msg);
 	},
 	
 	checkPassword: function()
 	{
 		var password = this.control.value;
 		
 		this.passLength = false;
 		
 		if (this.options.minimum_password_length == 0 || password.length >= this.options.minimum_password_length)
 		{
 			if (this.options.maximum_password_length == 0 || password.length <= this.options.maximum_password_length)
 			{
 				this.passLength = true;
 			}
 		}
 		
 		this.passDigits = false;
 		
 		if (this.options.require_digits)
 		{
 			if (password.match(/\d/)) this.passDigits = true;
 		}
 		else
 		{
 			this.passDigits = true;
 		}
 		
 		this.passUppercase = false;
 		
 		if (this.options.require_uppercase)
 		{
 			if (password.match(/[A-Z]/) && password.match(/[a-z]/))
 			{
 				this.passUppercase = true;
 			}
 		}
 		else
 		{
 			this.passUppercase = true;
 		}
 		
 		this.passSpecial = false;
 		
 		if (this.options.require_special)
 		{
 			if (password.match(/[^A-Za-z0-9]/))
 			{
 				this.passSpecial = true;
 			}
 		}
 		else
 		{
 			this.passSpecial = true;
 		}
 		
 		this.updatePanel();
 	}
});