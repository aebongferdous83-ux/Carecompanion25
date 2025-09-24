void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  device_name = String(device->getDeviceName());
  param_name = String(param->getParamName());
  incoming_message = 1;
  if(device_name == "Notify") received_message = val.val.s;
  else incoming_value = val.val.b;
  param->updateAndReport(val);
}