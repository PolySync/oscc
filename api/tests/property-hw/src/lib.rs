pub fn constrain(value: f64, min: f64, max: f64) -> f64 {
    if value < min {
        return min;
    }
    else if value > max {
        return max;
    }
    value
}