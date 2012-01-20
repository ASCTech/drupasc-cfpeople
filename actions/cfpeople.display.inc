<?php
// $Id$
/**
 * @file
 *
 * Handle all block related data
 *
 */

function people_webservice_search() {
  // Initialize the output variable
  $output = '';
  
  $people_types = _cfpeople_get_types();

  if (variable_get('cfpeople_toggle_stack', 0) == 1) {
    variable_set('cfpeople_sorting_types', 0);
  }
  $output .= '<div class="cfpeople-type-list">';
  $output .= '<ul>';
  
  if (variable_get('cfpeople_toggle_stack', 0) != 0) {
    $output .= '<li><a href="' . base_path() . 'people/listing" title="View the ' . variable_get('cfpeople_stack_title', 'Faculty') . ' listing.">' . variable_get('cfpeople_stack_title', 'Faculty') . '</a></li>';
  }
  
  foreach ($people_types as $type) {
    if (variable_get('cfpeople_sorting_types', 0) == 0 && variable_get('cfpeople_toggle_stack', 0) == 0) {
      if ($type['active'] != '0') {
        $output .= '<li><a href="' . base_path() . 'people/' . $type['valuename'] . '" title="' . $type['linktext'] . '">' . $type['displaytitle'] . '</a></li>';
      }
    }
    else {
      if ($type['active'] != '0' && $type['stackable'] != 1) {
        $output .= '<li><a href="' . base_path() . 'people/list/' . $type['valuename'] . '/a,f" title="' . $type['linktext'] . '">' . $type['displaytitle'] . '</a></li>';
      }
    }
  }
  
  $output .= '</ul>';
  $output .= '</div>';
    
  $output .= 'If you know a person\'s name, use the form below to find him or her. Or use one of the links above to display listings of people in the ' . variable_get('cfpeople_deptname', 'Department of English') . '.';
  
  $output .= drupal_get_form('cfpeople_webservice_search_form');
  
  return $output;
}

function cfpeople_webservice_search_form($form_state) {
  
  // initialize the tab index
  $tabindex = 1;
  
  // initialize the weight index
  $weightindex = 1;    
  
  // Define the people title or name
  $form['searchfield'] = array(
    '#type' => 'textfield',
    '#title' => t('<strong>Search by Last Name</strong> for a ' . variable_get('cfpeople_deptshort', 'English') . ' faculty, staff member or graduate student'),
    '#description' => t('Enter the last name of the person you are searching for.'),
    '#required' => TRUE,
    '#weight' => $weightindex++,
    '#attributes' => array('tabindex' => $tabindex++),
  );  
  
  // submit button
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#weight' => $weightindex++,
    '#attributes' => array('tabindex' => $tabindex++),
  );  
  
  // Return the form
  return $form;
}

function cfpeople_webservice_search_form_submit($form, &$form_state) {

  $keyword = $form_state['values']['searchfield'];
  
  unset($http_result);

  $ref_url = base_path() . 'people';
  
  $feedurl = 'http://mayanarchives-popolwuj.osu.edu/people_webservice/xml.cfm';
  $feedquery = '?t=4&id=190&list=user&deptidnum=' . variable_get('cfpeople_peopledept', '05540') . '&referrer=' . $ref_url . '&keyword=' . $keyword . '&lft=A&rgt=Z';
  
  if ($feedquery != '') {
    $feed = t($feedurl . $feedquery);
  }
  else {
    $feed = t($feedurl);
  }  
  
  $http_result = drupal_http_request($feed);

  //print_r($http_result);

  // Load up the XML File
  if ($http_result->code == 200) {
    $xmldoc = new DOMDocument();
    try {
      $xmldoc->loadXML($http_result->data);
    } 
    catch (DOMException $e) {      
      $msg = "Error parsing events XML for %url: %msg.";
      $vars = array('%url' => $url, '%msg' => $e->getMessage());
      watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
      return t("Getting the feed for events resulted in an error.");
    }
  } 
  else {    
    $msg = 'No content from %url.';
    $vars = array( '%url' => $url );
    watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
    return t("There are no people in this category.");
  }  
    
  $output .= decode_entities($xmldoc->saveXML());  
  
  $output .= '<br /><a href="' . base_path() . 'people/" title="">Return to the search page.</a>';
  
  print theme('page', $output);
  exit;

}

/**
 * Display data for people
 */
function people_webservice_list($peoplelist = 'Faculty', $disprange = 'A,Z') {
  $output = '';
  
  unset($http_result);
  
  $personid = '190';

  if (variable_get('cfpeople_sorting_types', 0) == 0) {
    $lft = 'a';
    $rgt = 'z';    
  }
  else {
    $lft = drupal_substr($disprange, 0, 1);
    $rgt = drupal_substr($disprange, -1);    
  }
	
  $ref_url = base_path() . 'people';

  $feedurl = 'http://mayanarchives-popolwuj.osu.edu/people_webservice/xml.cfm';
  $feedquery = '?t=2&id=' . $personid . '&list=' . $peoplelist . '&deptidnum=' . variable_get('cfpeople_peopledept', '05540') . '&referrer=' . $ref_url . '&keyword=NONE&lft=' . drupal_strtoupper($lft) . '&rgt=' . drupal_strtoupper($rgt);
  
  if ($feedquery != '') {
    $feed = $feedurl . $feedquery;
  }
  else {
    $feed = $feedurl;
  }
  
  $http_result = drupal_http_request($feed);

  if (variable_get('cfpeople_sorting_types', 0) != 0) {
    $output .= '<div class="cfpeople-sorting-list">';
    $output .= '<ul>';
    $output .= '<li><a href="' . base_path() . 'people/list/' . $peoplelist . '/' . variable_get('cfpeople_sorting_1_1', 'A') . ',' . variable_get('cfpeople_sorting_1_2', 'F') . '" title="View people with last names that start with ' . variable_get('cfpeople_sorting_1_1', 'A') . ' through ' . variable_get('cfpeople_sorting_1_2', 'F') . '">' . variable_get('cfpeople_sorting_1_1', 'A') . ' - ' . variable_get('cfpeople_sorting_1_2', 'F') . '</a></li>';
    $output .= '<li><a href="' . base_path() . 'people/list/' . $peoplelist . '/' . variable_get('cfpeople_sorting_2_1', 'G') . ',' . variable_get('cfpeople_sorting_2_2', 'L') . '" title="View people with last names that start with ' . variable_get('cfpeople_sorting_2_1', 'G') . ' through ' . variable_get('cfpeople_sorting_2_2', 'L') . '">' . variable_get('cfpeople_sorting_2_1', 'G') . ' - ' . variable_get('cfpeople_sorting_2_2', 'L') . '</a></li>';
    $output .= '<li><a href="' . base_path() . 'people/list/' . $peoplelist . '/' . variable_get('cfpeople_sorting_3_1', 'M') . ',' . variable_get('cfpeople_sorting_3_2', 'R') . '" title="View people with last names that start with ' . variable_get('cfpeople_sorting_3_1', 'M') . ' through ' . variable_get('cfpeople_sorting_3_2', 'R') . '">' . variable_get('cfpeople_sorting_3_1', 'M') . ' - ' . variable_get('cfpeople_sorting_3_2', 'R') . '</a></li>';
    $output .= '<li><a href="' . base_path() . 'people/list/' . $peoplelist . '/' . variable_get('cfpeople_sorting_4_1', 'S') . ',' . variable_get('cfpeople_sorting_4_2', 'Z') . '" title="View people with last names that start with ' . variable_get('cfpeople_sorting_4_1', 'S') . ' through ' . variable_get('cfpeople_sorting_4_2', 'Z') . '">' . variable_get('cfpeople_sorting_4_1', 'S') . ' - ' . variable_get('cfpeople_sorting_4_2', 'Z') . '</a></li>';
    $output .= '</ul>';
    $output .= '</div>';
  }  
  
  // Load up the XML File
  if ($http_result->code == 200) {
    $xmldoc = new DOMDocument();
    try {
      $xmldoc->loadXML($http_result->data);
    } 
    catch (DOMException $e) {      
      $msg = "Error parsing events XML for %url: %msg.";
      $vars = array('%url' => $url, '%msg' => $e->getMessage());
      watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
      return t("Getting the feed for events resulted in an error.");
    }
  } 
  else {    
    $msg = 'No content from %url.';
    $vars = array( '%url' => $url );
    watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
    return t("There are no people in this category.");
  }  
    
  $output .= '<div class="cfpeople-search-link"><a href="' . base_path() . 'people/" title="Search for people in the department.">Search for People</a></div>';  
    
  $output .= decode_entities($xmldoc->saveXML());  
  
  return $output;
}

function people_webservice_person($peoplelist, $personid = '190') {
  $output = '';
  
  $ref_url = base_path() . 'people/' . $peoplelist;

  $feedurl = 'http://mayanarchives-popolwuj.osu.edu/people_webservice/xml.cfm';
  $feedquery = '?t=1&id=' . $personid . '&list=' . $peoplelist . '&deptidnum=' . variable_get('cfpeople_peopledept', '05540') . '&referrer=' . $ref_url . '&keyword=NONE&lft=A&rgt=Z';
  
  if ($feedquery != '') {
    $feed = $feedurl . $feedquery;
  }
  else {
    $feed = $feedurl;
  }
  $http_result = drupal_http_request($feed);
  
  // Load up the XML File
  if ($http_result->code == 200) {
    $xmldoc = new DOMDocument();
    try {
      $xmldoc->loadXML($http_result->data);
    } 
    catch (DOMException $e) {      
      $msg = "Error parsing events XML for %url: %msg.";
      $vars = array('%url' => $url, '%msg' => $e->getMessage());
      watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
      return t("Getting the feed for events resulted in an error.");
    }
  } 
  else {    
    $msg = 'No content from %url.';
    $vars = array( '%url' => $url );
    watchdog('eventsdisplay', $msg, $vars, WATCHDOG_WARNING);
    return t("There are no people in this category.");
  }  
        
  $output .= decode_entities($xmldoc->saveXML());

  if (variable_get('cfpeople_sorting_types', 0) == 0) {
    $output .= '<br /><a href="' . base_path() . 'people/' . $peoplelist . '" title="">Return to people list.</a>';
  }
  else {
    $output .= '<br /><a href="' . base_path() . 'people/list/' . $peoplelist . '/a,f" title="">Return to people list.</a>';
  }
  return $output;
}

/*
 * Stacked listing of faculty, based on entries passed in administration panel
 */
function people_facstack() {
  // Initialize output variable
  $output = '';
  
  $people_types = _cfpeople_get_types();
  
  foreach ($people_types as $type) {
    if ($type['stackable'] == 1) {      
      $output .= people_webservice_list($type['valuename'], 'A,Z') . '<br /><hr />';
    }    
  }
 
  // Return the output
  return $output;
} // function people_multiblock()