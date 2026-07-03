/**
 * @module api
 * @description Pre-configured Axios instance for the NombaCrypt Shell Python API.
 * Includes JWT auth interceptor and standardised error handling.
 */

import axios, { AxiosError, type InternalAxiosRequestConfig } from 'axios';
import { API_BASE_URL } from './constants';

/**
 * Axios instance with base URL pointed at the Python backend.
 * All REST calls should go through this instance.
 */
const api = axios.create({
  baseURL: API_BASE_URL,
  timeout: 15_000,
  headers: {
    'Content-Type': 'application/json',
    Accept: 'application/json',
  },
});

/* ──────────────────────── Request Interceptor ────────────────────── */

api.interceptors.request.use(
  (config: InternalAxiosRequestConfig) => {
    if (typeof window !== 'undefined') {
      const token = localStorage.getItem('nombacrypt_token');
      if (token && config.headers) {
        config.headers.Authorization = `Bearer ${token}`;
      }
    }
    return config;
  },
  (error: AxiosError) => Promise.reject(error),
);

/* ──────────────────────── Response Interceptor ───────────────────── */

api.interceptors.response.use(
  (response) => response,
  (error: AxiosError) => {
    if (error.response?.status === 401) {
      // Token expired or invalid — clear and redirect
      if (typeof window !== 'undefined') {
        localStorage.removeItem('nombacrypt_token');
        window.location.href = '/login';
      }
    }
    return Promise.reject(error);
  },
);

export default api;
