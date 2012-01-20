<?php
// $Id$
/**
 * @file
 *
 * Administer fields for OSU:Pro output
 *
 */

function cfpeople_types_form($form_state) {
  $data = _cfpeople_get_types();
    
  $form = array();

  $form['my_items'] = array();
  $form['my_items']['#tree'] = TRUE;
  
  foreach ($data AS $values) {

    $id = $values['id'];
    $title = $values['title'];
    $active = $values['active'];
    $weight = $values['weight'];
    
    $form['my_items'][$id] = array(
      'title' => array(
        '#type' => 'markup',
        '#value' => $title,
      ),
      'active' => array(
        '#type' => 'checkbox',
        '#default_value' => $active,
      ),   
      'weight' => array(
        '#type' => 'weight',
        '#delta' => count($data),
        '#default_value' => $weight,
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

function cfpeople_types_form_submit($form, &$form_state) {
  //Update the database with the new values
  foreach ($form_state['values']['my_items'] as $item) {
    db_query("UPDATE {cfpeople} SET weight = '%s', active = '%s' WHERE id = '%s' LIMIT 1", array($item['weight'], $item['active'], $item['id']));    
  }
}

function theme_cfpeople_types_form($form) {

  drupal_add_tabledrag('my_draggable_table', 'order', 'sibling', 'weight-group');
  
  $header = array('Title', 'Active', 'Weight');
  foreach (element_children($form['my_items']) as $key) {
        
    $element = &$form['my_items'][$key];
    $element['weight']['#attributes']['class'] = 'weight-group';
    $element['itemid']['#attributes']['class'] = 'itemid';
    
    $row = array();
    $row[] = drupal_render($element['title']);
    $row[] = drupal_render($element['active']);
    $row[] = drupal_render($element['weight']) . drupal_render($element['id']);
    
    //Add a draggable class to every table row (<tr>)
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }

  $output = theme('table', $header, $rows, array('id' => 'my_draggable_table'));

  $output .= drupal_render($form);

  return $output;
}

