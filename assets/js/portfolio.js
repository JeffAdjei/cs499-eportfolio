/* CS 499 ePortfolio — premium interactions
 * Adapted from the newcomponents design system
 *   - Sparkle particle canvas (from SPARKLES)
 *   - Mouse-following radial glow (from HOVARGLOW)
 *   - Animated mesh gradient orbs (from HERO / SHADERS)
 *   - Intersection-based reveal transitions
 */
(function () {
  'use strict';

  // Respect reduced-motion
  const prefersReducedMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

  /* -------------------- Sparkle particle canvas -------------------- */
  function initSparkles() {
    const canvas = document.getElementById('sparkle-canvas');
    if (!canvas || prefersReducedMotion) return;
    const ctx = canvas.getContext('2d');
    let w = 0, h = 0, dpr = Math.min(window.devicePixelRatio || 1, 2);

    function resize() {
      const rect = canvas.getBoundingClientRect();
      w = rect.width; h = rect.height;
      canvas.width = Math.floor(w * dpr);
      canvas.height = Math.floor(h * dpr);
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    }
    resize();
    window.addEventListener('resize', resize);

    const count = Math.max(40, Math.floor((w * h) / 14000));
    const particles = Array.from({ length: count }, () => ({
      x: Math.random() * w,
      y: Math.random() * h,
      r: Math.random() * 1.4 + 0.3,
      a: Math.random() * Math.PI * 2,
      s: Math.random() * 0.25 + 0.05,
      alpha: Math.random() * 0.6 + 0.2,
      pulse: Math.random() * 0.02 + 0.005,
    }));

    function tick() {
      ctx.clearRect(0, 0, w, h);
      for (const p of particles) {
        p.a += p.pulse;
        p.x += Math.cos(p.a) * p.s;
        p.y += Math.sin(p.a * 0.7) * p.s;
        if (p.x < -2) p.x = w + 2;
        if (p.x > w + 2) p.x = -2;
        if (p.y < -2) p.y = h + 2;
        if (p.y > h + 2) p.y = -2;
        const twinkle = 0.5 + 0.5 * Math.sin(p.a * 2);
        ctx.globalAlpha = p.alpha * twinkle;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.r, 0, Math.PI * 2);
        ctx.fillStyle = '#cfe9ff';
        ctx.shadowColor = '#7dd3fc';
        ctx.shadowBlur = 6;
        ctx.fill();
      }
      ctx.globalAlpha = 1;
      ctx.shadowBlur = 0;
      requestAnimationFrame(tick);
    }
    tick();
  }

  /* -------------------- Mouse-tracked glow -------------------- */
  function initGlowCards() {
    const cards = document.querySelectorAll('[data-glow]');
    cards.forEach((card) => {
      card.addEventListener('pointermove', (e) => {
        const rect = card.getBoundingClientRect();
        const x = ((e.clientX - rect.left) / rect.width) * 100;
        const y = ((e.clientY - rect.top) / rect.height) * 100;
        card.style.setProperty('--mx', x + '%');
        card.style.setProperty('--my', y + '%');
      });
      card.addEventListener('pointerleave', () => {
        card.style.setProperty('--mx', '50%');
        card.style.setProperty('--my', '120%');
      });
    });
  }

  /* -------------------- Reveal on scroll -------------------- */
  function initReveal() {
    const els = document.querySelectorAll('[data-reveal]');
    if (!('IntersectionObserver' in window) || prefersReducedMotion) {
      els.forEach((el) => el.classList.add('is-visible'));
      return;
    }
    const io = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting) {
            entry.target.classList.add('is-visible');
            io.unobserve(entry.target);
          }
        });
      },
      { threshold: 0.12 }
    );
    els.forEach((el) => io.observe(el));
  }

  /* -------------------- Year -------------------- */
  function initYear() {
    const yr = document.getElementById('year');
    if (yr) yr.textContent = new Date().getFullYear();
  }

  document.addEventListener('DOMContentLoaded', () => {
    initSparkles();
    initGlowCards();
    initReveal();
    initYear();
  });
})();
