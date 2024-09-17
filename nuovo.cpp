
int main ()
 {
  uTimer u;
  disabletriggers();
  unlock_trigger();

  enable_triggers();

  bool in_spill(false), trigger_present(false);
  uint32_t n_events(0);

  u.setOffset();
  u.start();
  while (1)
   {
    trigger_mask = read_mask();
    if (trigger_mask != 0)
     {
      double t = u.elapsedSeconds();
      if (!in_spill || (t>5)) spill_number++;
      read_event();
      in_spill = true;
      n_events++;
      save_event();
      u.start();
     }
    else if (t > 3)
     {
     }
   }
  return 0;
 }
