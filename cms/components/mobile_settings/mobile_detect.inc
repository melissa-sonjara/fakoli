<?php
/**\addtogroup mobile_settings
 * @{
 */


/**
 * Mobile Detect
 * $Id: Mobile_Detect.php 39 2012-04-15 14:53:13Z serbanghita@gmail.com $
 * 
 * @usage      $detect = new Mobile_Detect();
 *             $detect->isMobile() or $detect->isTablet()
 * 
 *             For more specific usage see the documentation inside the class.
 *             $detect->isAndroidOS() or $detect->isiPhone() ...
 * 
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */

class MobileDetect 
{    
    protected $detectionRules;
    protected $userAgent = null;
    protected $accept = null;
    // Assume the visitor has a desktop environment.
    protected $isMobile = false;
    protected $isTablet = false;
    protected $phoneDeviceName = null;
    protected $tabletDevicename = null;
    protected $operatingSystemName = null;
    protected $userAgentName = null;
    // List of mobile devices (phones)
    protected $phoneDevices = array(     
            'iPhone' => '(iPhone.*Mobile|iPod|iTunes)',            
            'BlackBerry' => 'BlackBerry|rim[0-9]+',
            'HTC' => 'HTC|Desire',
            'Nexus' => 'Nexus One|Nexus S',
            'DellStreak' => 'Dell Streak',
            'Motorola' => '\bDroid\b.*Build|HRI39|MOT\-',
            'Samsung' => 'Samsung|GT-I9100|GT-I9000|GT-I9020|SCH-A310|SCH-A530|SCH-A570|SCH-A610|SCH-A630|SCH-A650|SCH-A790|SCH-A795|SCH-A850|SCH-A870|SCH-A890|SCH-A930|SCH-A950|SCH-A970|SCH-A990|SCH-I100|SCH-I110|SCH-I400|SCH-I405|SCH-I500|SCH-I510|SCH-I515|SCH-I600|SCH-I730|SCH-I760|SCH-I770|SCH-I830|SCH-I910|SCH-I920|SCH-LC11|SCH-N150|SCH-N300|SCH-R300|SCH-R400|SCH-R410|SCH-T300|SCH-U310|SCH-U320|SCH-U350|SCH-U360|SCH-U365|SCH-U370|SCH-U380|SCH-U410|SCH-U430|SCH-U450|SCH-U460|SCH-U470|SCH-U490|SCH-U540|SCH-U550|SCH-U620|SCH-U640|SCH-U650|SCH-U660|SCH-U700|SCH-U740|SCH-U750|SCH-U810|SCH-U820|SCH-U900|SCH-U940|SCH-U960|SCS-26UC|SGH-A107|SGH-A117|SGH-A127|SGH-A137|SGH-A157|SGH-A167|SGH-A177|SGH-A187|SGH-A197|SGH-A227|SGH-A237|SGH-A257|SGH-A437|SGH-A517|SGH-A597|SGH-A637|SGH-A657|SGH-A667|SGH-A687|SGH-A697|SGH-A697|SGH-A707|SGH-A717|SGH-A727|SGH-A737|SGH-A747|SGH-A767|SGH-A777|SGH-A797|SGH-A817|SGH-A827|SGH-A837|SGH-A847|SGH-A867|SGH-A877|SGH-A887|SGH-A897|SGH-A927|SGH-C207|SGH-C225|SGH-C417|SGH-D307|SGH-D347|SGH-D357|SGH-D407|SGH-D415|SGH-D807|SGH-E105|SGH-E315|SGH-E316|SGH-E317|SGH-E335|SGH-E635|SGH-E715|SGH-I577|SGH-I607|SGH-I617|SGH-I627|SGH-I637|SGH-I677|SGH-I717|SGH-I727|SGH-I777|SGH-I827|SGH-I847|SGH-I857|SGH-I897|SGH-I907|SGH-I917|SGH-I927|SGH-I937|SGH-I997|SGH-N105|SGH-N625|SGH-P107|SGH-P207|SGH-P735|SGH-P777|SGH-Q105|SGH-R225|SGH-S105|SGH-S307|SGH-T109|SGH-T119|SGH-T139|SGH-T209|SGH-T219|SGH-T229|SGH-T239|SGH-T249|SGH-T259|SGH-T309|SGH-T319|SGH-T329|SGH-T339|SGH-T349|SGH-T359|SGH-T369|SGH-T379|SGH-T409|SGH-T429|SGH-T439|SGH-T459|SGH-T469|SGH-T479|SGH-T499|SGH-T509|SGH-T519|SGH-T539|SGH-T559|SGH-T589|SGH-T609|SGH-T619|SGH-T629|SGH-T639|SGH-T659|SGH-T669|SGH-T679|SGH-T709|SGH-T719|SGH-T729|SGH-T739|SGH-T749|SGH-T759|SGH-T769|SGH-T809|SGH-T819|SGH-T839|SGH-T919|SGH-T919|SGH-T929|SGH-T939|SGH-T939|SGH-T959|SGH-T989|SGH-V205|SGH-V206|SGH-X105|SGH-X426|SGH-X427|SGH-X475|SGH-X495|SGH-X497|SGH-X507|SGH-ZX10|SGH-ZX20|SPH-A120|SPH-A400|SPH-A420|SPH-A460|SPH-A500I|SPH-A560|SPH-A600|SPH-A620|SPH-A660|SPH-A700|SPH-A740|SPH-A760|SPH-A790|SPH-A800|SPH-A820|SPH-A840|SPH-A880|SPH-A900|SPH-A940|SPH-A960|SPH-D600|SPH-D700|SPH-D710|SPH-D720|SPH-I300|SPH-I325|SPH-I330|SPH-I350|SPH-I500|SPH-I600|SPH-I700|SPH-L700|SPH-M100|SPH-M220|SPH-M240|SPH-M300|SPH-M305|SPH-M320|SPH-M330|SPH-M350|SPH-M360|SPH-M370|SPH-M380|SPH-M510|SPH-M540|SPH-M550|SPH-M560|SPH-M570|SPH-M580|SPH-M610|SPH-M620|SPH-M630|SPH-M800|SPH-M810|SPH-M850|SPH-M900|SPH-M910|SPH-M920|SPH-M930|SPH-N200|SPH-N240|SPH-N300|SPH-N400|SPH-Z400|SWC-E100',
            'Sony' => 'E10i|SonyEricsson|SonyEricssonLT15iv',
            'Asus' => 'Asus.*Galaxy',
            'Palm' => 'PalmSource|Palm', // avantgo|blazer|elaine|hiptop|plucker|xiino
            'GenericPhone' => '(mmp|pocket|psp|symbian|Smartphone|smartfon|treo|up.browser|up.link|vodafone|wap|nokia|Series40|Series60|S60|SonyEricsson|N900|MAUI.*WAP.*Browser|LG\-P500)'
    );
    // List of tablet devices.
    protected $tabletDevices = array(
        'BlackBerryTablet' => 'PlayBook|RIM Tablet',
        'iPad' => 'iPad.*Mobile',
        'Kindle' => 'Kindle|Silk.*Accelerated',
        'SamsungTablet' => 'SAMSUNG.*Tablet|Galaxy.*Tab|GT-P1000|GT-P1010|GT-P6210|GT-P6800|GT-P6810|GT-P7100|GT-P7300|GT-P7310|GT-P7500|GT-P7510|SCH-I800|SCH-I815|SCH-I905|SGH-I777|SGH-I957|SGH-I987|SGH-T849|SGH-T859|SGH-T869|SGH-T989|SPH-D710|SPH-P100',
        'MotorolaTablet' => 'xoom|sholest',
        'AsusTablet' => 'Transformer|TF101',
        'NookTablet' => 'NookColor|nook browser|BNTV250A|LogicPD Zoom2',
        'AcerTablet' => 'Android.*(A100|A101|A200|A500|A501|A510|W500|W500P|W501|W501P)',
        'GenericTablet' => 'Tablet|ViewPad7|LG\-V909|MID7015|BNTV250A|LogicPD Zoom2|\bA7EB\b|CatNova8|A1_07|CT704|CT1002|\bM721\b',
    );
    // List of mobile Operating Systems.
    protected $operatingSystems = array(
        'AndroidOS' => '(android.*mobile|android(?!.*mobile))',
        'BlackBerryOS' => '(blackberry|rim tablet os)',
        'PalmOS' => '(avantgo|blazer|elaine|hiptop|palm|plucker|xiino)',
        'SymbianOS' => 'Symbian|SymbOS|Series60|Series40|\bS60\b',
        'WindowsMobileOS' => 'IEMobile|Windows Phone|Windows CE.*(PPC|Smartphone)|MSIEMobile|Window Mobile|XBLWP7',
        'iOS' => '(iphone|ipod|ipad)',
        'FlashLiteOS' => '',
        'JavaOS' => '',
        'NokiaOS' => '',
        'webOS' => '',
        'badaOS' => '\bBada\b',
        'BREWOS' => '',
    );
    // List of mobile User Agents.
    protected $userAgents = array(      
      'Chrome' => '\bCrMo\b',
      'Dolfin' => '\bDolfin\b',
      'Opera' => '(Opera.*Mini|Opera.*Mobi)',  
      'Skyfire' => 'skyfire',      
      'IE' => 'ie*mobile',
      'Firefox' => 'fennec|firefox.*maemo',
      'Bolt' => 'bolt',
      'TeaShark' => 'teashark',
      'Blazer' => 'Blazer',
      'Safari' => 'Mobile*Safari',
      'Midori' => 'midori',
      'GenericBrowser' => 'NokiaBrowser|OviBrowser'
    );
    
    function __construct(){
        
        // Merge all rules together.
        $this->detectionRules = array_merge(
                                            $this->phoneDevices, 
                                            $this->tabletDevices, 
                                            $this->operatingSystems, 
                                            $this->userAgents
                                            );
        $this->userAgent = $_SERVER['HTTP_USER_AGENT'];
        $this->accept = $_SERVER['HTTP_ACCEPT'];  
        
        if (
		isset($_SERVER['HTTP_X_WAP_PROFILE']) ||
                isset($_SERVER['HTTP_X_WAP_CLIENTID']) ||
                isset($_SERVER['HTTP_WAP_CONNECTION']) ||
		isset($_SERVER['HTTP_PROFILE']) ||
		isset($_SERVER['HTTP_X_OPERAMINI_PHONE_UA']) || // Reported by Nokia devices (eg. C3)
                isset($_SERVER['HTTP_X_NOKIA_IPADDRESS']) ||
                isset($_SERVER['HTTP_X_NOKIA_GATEWAY_ID']) ||
                isset($_SERVER['HTTP_X_ORANGE_ID']) ||
                isset($_SERVER['HTTP_X_VODAFONE_3GPDPCONTEXT']) ||
                isset($_SERVER['HTTP_X_HUAWEI_USERID']) ||
                isset($_SERVER['HTTP_UA_OS']) || // Reported by Windows Smartphones
                (isset($_SERVER['HTTP_UA_CPU']) && $_SERVER['HTTP_UA_CPU'] == 'ARM') // Seen this on a HTC
		) {
                $this->isMobile = true;
        } elseif (!empty($this->accept) && (strpos($this->accept, 'text/vnd.wap.wml') !== false || strpos($this->accept, 'application/vnd.wap.xhtml+xml') !== false)) {
                $this->isMobile = true;
        } else {
                $this->_detect();
        }        
        
    }
	
    public function getRules()
    {
        return $this->detectionRules;
    }
    
    /**
     * 
     * @method boolean isiPhone()
     * @method boolean isBlackBerry()
     * @method boolean isHTC()
     * @method boolean isNexus()
     * @method boolean isDellStreak()
     * @method boolean isMotorola()
     * @method boolean isSamsung()
     * @method boolean isSony()
     * @method boolean isAsus()
     * @method boolean isPalm()
     *
     * @method boolean isBlackBerryTablet()
     * @method boolean isiPad()
     * @method boolean isKindle()
     * @method boolean isSamsungTablet()
     * @method boolean isMotorolaTablet()
     * @method boolean isAsusTablet()
     *       
     * @method boolean isAndroidOS()
     * @method boolean isBlackBerryOS()
     * @method boolean isPalmOS()
     * @method boolean isSymbianOS()
     * @method boolean isWindowsMobileOS()
     * @method boolean isiOS()
     * 
     * @method boolean isChrome()
     * @method boolean isDolfin()
     * @method boolean isOpera()
     * @method boolean isSkyfire()
     * @method boolean isIE()
     * @method boolean isFirefox()
     * @method boolean isBolt()
     * @method boolean isTeaShark()
     * @method boolean isBlazer()
     * @method boolean isSafari()
     * @method boolean isMidori()
     *   
     * @param string $name
     * @param array $arguments
     * @return mixed 
     */
    public function __call($name, $arguments)
    {
                
        $key = substr($name, 2);
        return $this->_detect($key);
        
    }
	
    private function _detect($key='')
    {

        if(empty($key)){ 

            // Begin general search.
            foreach($this->detectionRules as $_key => $_regex){
                if(empty($_regex)){ continue; }
                if(preg_match('/'.$_regex.'/is', $this->userAgent)){
                    $this->isMobile = true;
                    return true;
                } 
            }
            return false;

        } else {
            
            // Search for a certain key.
            // Make the keys lowecase so we can match: isIphone(), isiPhone(), isiphone(), etc.
            $key = strtolower($key);
            $_rules = array_change_key_case($this->detectionRules);
            
            if(array_key_exists($key, $_rules)){
                if(empty($_rules[$key])){ return null; }
                if(preg_match('/'.$_rules[$key].'/is', $this->userAgent)){
                    $this->isMobile = true;
                    return true;
                } else {
                    return false;
                }           
            } else {
                trigger_error("Method $key is not defined", E_USER_WARNING);
            }
            
            return false;

        }

    }
        
    /**
    * Returns true if any type of mobile device detected, including special ones
    * @return bool
    */
    public function isMobile()
    {
            return $this->isMobile;
    } 
    
    /**
     * Return true if any type of tablet device is detected.
     * @return boolean 
     */
    public function isTablet()
    {
        
        foreach($this->tabletDevices as $_key => $_regex){
            if(preg_match('/'.$_regex.'/is', $this->userAgent)){
                $this->isTablet = true;
                return true;
            }
        }
        
        return false;        
            
    }
    
    
}
/**@}*/?>