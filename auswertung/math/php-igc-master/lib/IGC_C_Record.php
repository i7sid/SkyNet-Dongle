<?php
/**
 * IGC_C_Record class extends IGC_Record. The C record is task data
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_C_Record extends IGC_Record
{
  /**
   * Record Time
   *
   * @access public
   * @var date
   */
  public $time;

  /**
   * Flight Date
   *
   * @access public
   * @var date
   */
  public $flight_date;

  /**
   * Task ID
   *
   * @access public
   * @var integer
   */
  public $task_id;

  /**
   * Number of turning points in the task
   *
   * @access public
   * @var integer
   */
  public $number_of_tps;

  /**
   * Latitude array [0] degrees, [1] minutes, [2] decimal minutes, [3] direction
   *
   * @access public
   * @var array
   */
  public $latitude;

  /**
   * Longitude array [0] degrees, [1] minutes, [2] decimal minutes, [3] direction
   *
   * @access public
   * @var array
   */
  public $longitude;

  /**
   * Comment
   *
   * @access public
   * @var string
   */
  public $comment;

  /**
   * Class constructor creates the C record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'C';
    $this->raw = $record;

    // turning point
    if (preg_match('/^[0-9]{7}[N|S]{1}[0-9]{8}[E|W]{1}/',substr($record,1))) {

      // set degrees, minutes, and decimal minutes
      // latitude
      $this->latitude = array();
      $this->latitude['degrees'] = substr($record,1,2);
      $this->latitude['minutes'] = substr($record,3,2);
      $this->latitude['decimal_minutes'] = substr($record,5,3);
      $this->latitude['direction'] = substr($record,8,1);

      // longitude
      $this->longitude = array();
      $this->longitude['degrees'] = substr($record,9,3);
      $this->longitude['minutes'] = substr($record,12,2);
      $this->longitude['decimal_minutes'] = substr($record,14,3);
      $this->longitude['direction'] = substr($record,17,1);

      $this->comment = substr($record,18);
    }
    // new task
    else {
      $d = substr($record,1,2);
      $mo = substr($record,3,2);
      $y = substr($record,5,2);
      $h = substr($record,7,2);
      $m = substr($record,9,2);
      $s = substr($record,11,2);

      $this->time = strtotime($mo."/".$d."/".$y." ".$h.":".$m.":".$s);

      $d = substr($record,13,2);
      $mo = substr($record,15,2);
      $y = substr($record,17,2);

      $this->flight_date = strtotime($mo."/".$d."/".$y);

      $this->task_id = substr($record,19,4);
      $this->number_of_tps = substr($record,23,2);
      $this->comment = substr($record,25);
    }
  }
}
?>
