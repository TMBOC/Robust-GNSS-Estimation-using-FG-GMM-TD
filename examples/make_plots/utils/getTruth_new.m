function [truth_t] = getTruth_new(ref)

truth_t = mean(ref(:, 2:end));
