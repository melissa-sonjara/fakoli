<h2>Welcome to the Fakoli Site Administration Interface</h2>
<p>The Fakoli Administration interface is designed to provide you
with all the screens you need to maintain the content and configuration
of your Fakoli web application.</p>
<p>On the left hand side you will see an accordian menu that groups
the administration screens by category. Clicking on a category (such
as <b>Web Site Content</b>) will display the options available within
that category. Please note that since Fakoli is a component-based system
both the options and the categories available can differ between sites
depending on the installed components.</p>
<p>The right hand side of the administration home page shows some useful
real-time statistics about the current server load and available disk space.
Additionally there are three utility buttons:</p>
<ul>
 <li>The <b>Scan for Component Updates</b> button initiates a component scan.
 You can use this button to make Fakoli aware of any component changes you
 have made, such as updating a component to a newer version, or adding an
 entirely new component to the system.</li>
 <li>The <b>Show PHP Configuration</b> button displays detailed configuration
 information regarding the version of PHP installed on your server, along with
 the configuration settings in your php.ini file and details of all loaded 
 extensions. This information is provided by the <tt>phpinfo()</tt> function
 and is only accessible when logged in as an admin user.</li>
 <li>Finally the <b>Create a New User</b> button takes you straight to the
 new user screen in the user management section.</li>
</ul>
<h3>Component Configuration Signature</h3>
<p>In addition to the above information, the page also displays a unique key
that corresponds to the versions of each installed, enabled component in the
current Fakoli configuration. This key can be used to help you determine whether
two Fakoli instances are running the exact same code configuration. This can
be especially useful as a sanity check when deploying software from a development
server to a production server.</p>