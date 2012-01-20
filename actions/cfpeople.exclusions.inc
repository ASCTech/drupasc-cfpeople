<?php
// $Id$
/**
 * @file
 *
 * Administer fields for OSU:Pro output
 *
 */

function cfpeople_exclusions_form($form_state) {
  $data = _cfpeople_get_types();
    
  $form = array();

  $form['my_items'] = array();
  $form['my_items']['#tree'] = TRUE;

  foreach ($data AS $values) {

    $id = $values['id'];
    $title = $values['title'];
    $stackable = $values['stackable'];
    
    $form['my_items'][$id] = array(
      'title' => array(
        '#type' => 'markup',
        '#value' => $title,
      ),
      'stackable' => array(
        '#type' => 'checkbox',
        '#default_value' => $stackable,
      ),   
      'id' => array(
        '#type' => 'hidden',
        '#value' => $id,
      ),       
    );
  }

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),   
  );

  return $form;
}

function cfpeople_exclusions_form_submit($form, &$form_state) {
  //Update the database with the new values
  foreach ($form_state['values']['my_items'] as $item) {
    db_query("UPDATE {cfpeople} SET stackable = '%s' WHERE id = '%s' LIMIT 1", array($item['stackable'], $item['id']));    
  }
}

function theme_cfpeople_exclusions_form($form) {

  $output = '';
  
  foreach (element_children($form['my_items']) as $key) {
    $element = &$form['my_items'][$key];
    $output .= '<div class="exclusions-form">';
    $output .= drupal_render($element['stackable']);
    $output .= drupal_render($element['title']);
    $output .= '</div>'; 
  }
  $output .= '<div class="description">Select the people types which you would like to have appear under the faculty link.</div>';
  
  $output .= drupal_render($form);

  return $output;
}