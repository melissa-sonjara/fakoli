<h2>How to use the Auto-Login Component</h2>
<p>The auto-login component provides an authentication mechanism that allows
access to a Fakoli site under a user's credential with a known token. This token
can be single- or multi-use, can optionally be limited to only allow authentication
from a specified IP address (or list of IP addresses). The token can also be associated
with a specific URI within the Fakoli site, to which processing is transferred after
the token has been verified.</p>
<p>Uses for the auto-login component include:</p>
 <ul>
  <li>Cross-domain sign-on between Fakoli sites</li>
  <li>Access to user-restricted functionality from a command-line script,
  such as a cron job or Windows scheduled task.</li>
  <li>Simple token-based access to a site within a LAN</li>  
 </ul>
<p>For example, to use an authentication token to run an action from the command-line, 
you would first create the token on the Authentication Token form, specifying the user account, 
the IP address range, leaving "Single-Use" unchecked (so the token can be used more than once),
checking "Enabled" and setting the "Redirect on Success" field to have the name of the 
action handler you wish to call after authentication. The form is prepopulated with a 
securely generated unique token to identify these access permissions. The generated tokens are 
32-character hexadecimal strings, based on a secure hash and timestamp. You can optionally 
override this token with a shorter or more human-readable token, but this is not recommended.</p>

<p>Once you have the token, you can then make a call from the command line using (for example)
the <b>wget</b> utility on Linux:
<pre>
wget -O http://mysite/action/auto_login/authenticate?token=8fcf1a93f799...
</pre>
<p>The action handler you populated in the Redirect On Success field will then be called under 
the user credentials of the user associated with the token.</p>