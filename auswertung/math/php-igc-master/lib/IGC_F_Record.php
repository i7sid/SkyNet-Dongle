<?php
/**
 * IGC_F_Record class extends IGC_Record. Satellite constelation.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_F_Record extends IGC_Record
{
  /**
   * Time array [0] hours, [1] minutes, [3] seconds
   *
   * @access public
   * @var array
   */
  public $time_array;

  /**
   * Satellite ID
   *
   * @access public
   * @var string
   */
  public $satellite_id;

  /**
   * Comments
   *
   * @access public
   * @var string
   */
  public $comment;

  /**
   * Class constructor creates the F record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'F';
    $this->raw = $record;

    $this->time_array['h'] = substr($record, 1, 2);
    $this->time_array['m'] = substr($record, 3, 2);
    $this->time_array['s'] = substr($record, 5, 2);

  }
}
?>
